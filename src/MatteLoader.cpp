#include <MatteLoader.hpp>

#include <algorithm>

#include <OpenImageIO/imageio.h>

namespace charm
{

static u32 max_loaded_frames = 5;

namespace fs = std::filesystem;

MatteLoader::MatteLoader ()
  : m_matte_dir {},
    m_keep_running {false},
    m_dir_frame_count {0},
    m_requested_offset {0}
{
}

MatteLoader::~MatteLoader ()
{
  Shutdown ();

  for (MatteFrame &frame : m_frames)
    free (frame.data);

  m_frames.clear ();
}

void MatteLoader::StartLoading (std::filesystem::path const &_dir)
{
  m_matte_dir = _dir;

  m_dir_frame_count = std::distance (fs::directory_iterator {m_matte_dir},
                                     fs::directory_iterator {});

  m_matte_paths.insert(m_matte_paths.end (),
                       fs::directory_iterator {m_matte_dir},
                       fs::directory_iterator {});
  std::sort (m_matte_paths.begin (), m_matte_paths.end ());

  m_requested_offset = 0u;
  m_keep_running.store (true);
  m_thread = std::thread ([this] () {MatteLoader::LoadFrames(this); });
}

void MatteLoader::Shutdown ()
{
  m_keep_running.store (false);
  m_frame_cond.notify_all();
  m_thread.join();
}


MatteFrame MatteLoader::GetFrame (u32 _offset)
{
  std::unique_lock lock {m_frame_mutex};
  auto end = m_frames.end ();
  for (auto cur = m_frames.begin(); cur != end; ++cur)
    {
      if (cur->offset == _offset)
        {
          MatteFrame frame = std::exchange (*cur, MatteFrame {});
          std::for_each (m_frames.begin (), cur+1,
                         [] (MatteFrame &f) { free (f.data); });
          m_frames.erase (m_frames.begin (), cur+1);
          m_requested_offset = _offset;
          m_frame_cond.notify_one ();
          return frame;
        }
    }

  m_frame_cond.notify_one();
  return {};
}

static u32 ring_distance (u32 _cur, u32 _last, u32 _max)
{
  if (_cur <= _last)
    return _last - _cur;

  return (_max - _cur) + _last;
}

static u32 ring_next (u32 _cur, u32 _last, u32 _max)
{
  u32 l = _last % _max;
  if (ring_distance (_cur, l, _max) == 0)
    return u32 (-1);

  return (_cur + 1) % _max;
}

void MatteLoader::LoadFrames (MatteLoader *_loader)
{
  while (_loader->m_keep_running.load ())
    {
      //determine next matte to load
      //u32 max signifies no loading is required right now
      u32 next = u32 (-1);

      std::unique_lock lock {_loader->m_frame_mutex};

      //if we've loaded no mattes, load the last requested matte
      if (_loader->m_frames.empty())
        next = (_loader->m_requested_offset) % _loader->m_dir_frame_count;
      else //otherwise load the matte after the previously loaded matte
        next = ring_next (_loader->m_frames.back ().offset, _loader->m_requested_offset + max_loaded_frames,
                          _loader->m_dir_frame_count);

      if (next == u32 (-1)) //wait if we've loaded enough buffers
        {
          _loader->m_frame_cond.wait(lock);
          continue;
        }
      lock.unlock ();

      //ok, actually load from disk
      fs::directory_entry matte_on_deck = _loader->m_matte_paths[next];
      if (matte_on_deck.path().empty())
        {
          fprintf (stderr, "empty path on loading %s\n", _loader->m_matte_dir.c_str());
          continue;
        }

      bgfx::TextureFormat::Enum const formats[5]
        { bgfx::TextureFormat::Unknown,
          bgfx::TextureFormat::R8,
          bgfx::TextureFormat::RG8,
          bgfx::TextureFormat::RGB8,
          bgfx::TextureFormat::RGBA8
        };

      std::unique_ptr<OIIO::ImageInput> iinput
        = OIIO::ImageInput::open (matte_on_deck.path ().string ());

      if (! iinput)
        {
          fprintf (stderr, "error loading %s\n", matte_on_deck.path ().c_str());
          continue;
        }

      OIIO::ImageSpec ispec = iinput->spec ();
      i32 const max_dim = 1 << 15;
      i32 const max_channels = 4;
      assert (ispec.width <= max_dim && ispec.height <= max_dim && ispec.nchannels < max_channels);

      u32 size = ispec.width * ispec.height * ispec.nchannels;
      u8 *data = (u8 *)malloc (size);

      iinput->read_image (OIIO::TypeDesc::UINT8, data);

      MatteFrame frame {next, formats[ispec.nchannels],
                        u32(ispec.width), u32(ispec.height), size, data};
      lock.lock ();
      _loader->m_frames.push_back (frame);
      //fprintf (stderr, "loaded %u\n", next);
    }
}

void MatteLoader::MatteDataReleaseFn (void *_ptr, void *)
{
  free (_ptr);
}

}
