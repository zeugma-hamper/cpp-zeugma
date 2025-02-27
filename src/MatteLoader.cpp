
//
// (c) treadle & loam, provisioners llc
//

#include <MatteLoader.hpp>

#include <algorithm>
#include <chrono>

#include <OpenImageIO/imageio.h>


namespace zeugma  {

static u32 max_loaded_frames = 3;

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

u32 MatteLoader::FindNextOffset ()
{
  std::unique_lock lock {m_frame_mutex};
  while (m_keep_running.load ())
    {
      //if we've loaded no mattes, load the last requested matte
      if (m_frames.empty())
        return (m_requested_offset) % m_dir_frame_count;
      else //otherwise load the matte after the previously loaded matte
        {
          u32 next = ring_next (m_frames.back ().offset, m_requested_offset + max_loaded_frames,
                                m_dir_frame_count);
          if (next != u32 (-1))
            return next;
        }

      //wait if we've loaded enough buffers
      m_frame_cond.wait(lock);
    }

  return u32(-1);
}

void MatteLoader::StoreMatte (MatteFrame const &_mf)
{
  std::unique_lock lock {m_frame_mutex};
  m_frames.push_back (_mf);
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

  // didn't find what we're looking for so trash everything
  m_requested_offset = _offset;
  std::for_each (m_frames.begin (), m_frames.end (),
                 [] (MatteFrame &f) { free (f.data); });
  m_frames.clear ();
  m_frame_cond.notify_one();
  return {};
}

void MatteLoader::LoadFrames (MatteLoader *_loader)
{
  while (_loader->m_keep_running.load ())
    {
      //determine next matte to load
      //u32 max signifies no loading is required right now
      u32 next = _loader->FindNextOffset();

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
      iinput->close ();

      MatteFrame frame {next, formats[ispec.nchannels],
                        u32(ispec.width), u32(ispec.height), size, data};
      _loader->StoreMatte(frame);
    }
}

void MatteLoader::MatteDataReleaseFn (void *_ptr, void *)
{
  free (_ptr);
}

MatteLoaderBimg::MatteLoaderBimg ()
  : m_matte_dir {},
    m_keep_running {false},
    m_dir_frame_count {0},
    m_requested_offset {0}
{
}

MatteLoaderBimg::~MatteLoaderBimg ()
{
  Shutdown ();

  m_frames.clear ();
}

void MatteLoaderBimg::StartLoading (std::filesystem::path const &_dir)
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
  m_thread = std::thread ([this] () {MatteLoaderBimg::LoadFrames(this); });
}

void MatteLoaderBimg::Shutdown ()
{
  m_keep_running.store (false);
  m_frame_cond.notify_all();
  m_thread.join();
}

u32 MatteLoaderBimg::FindNextOffset ()
{
  std::unique_lock lock {m_frame_mutex};
  while (true)
    {
      //if we've loaded no mattes, load the last requested matte
      if (m_frames.empty())
        return (m_requested_offset) % m_dir_frame_count;
      else //otherwise load the matte after the previously loaded matte
        {
          u32 next = ring_next (m_frames.back ().offset, m_requested_offset + max_loaded_frames,
                                m_dir_frame_count);
          if (next != u32 (-1))
            return next;
        }

      //wait if we've loaded enough buffers
      m_frame_cond.wait(lock);
    }

  return u32(-1);
}

void MatteLoaderBimg::StoreMatte (MatteFrameBimg &&_mf)
{
  std::unique_lock lock {m_frame_mutex};
  m_frames.push_back (std::move (_mf));
}

MatteFrameBimg MatteLoaderBimg::GetFrame (u32 _offset)
{
  std::unique_lock lock {m_frame_mutex};
  auto end = m_frames.end ();
  for (auto cur = m_frames.begin(); cur != end; ++cur)
    {
      if (cur->offset == _offset)
        {
          MatteFrameBimg frame = std::exchange (*cur, MatteFrameBimg {});
          m_frames.erase (m_frames.begin (), cur+1);
          m_requested_offset = _offset;
          m_frame_cond.notify_one ();
          return frame;
        }
    }

  // didn't find what we're looking for so trash everything
  m_requested_offset = _offset;
  m_frames.clear ();
  m_frame_cond.notify_one();
  return {};
}

void MatteLoaderBimg::LoadFrames (MatteLoaderBimg *_loader)
{
  while (_loader->m_keep_running.load ())
    {
      //determine next matte to load
      //u32 max signifies no loading is required right now
      u32 next = _loader->FindNextOffset();

      //ok, actually load from disk
      fs::directory_entry matte_on_deck = _loader->m_matte_paths[next];
      if (matte_on_deck.path().empty())
        {
          fprintf (stderr, "empty path on loading %s\n", _loader->m_matte_dir.c_str());
          continue;
        }

      bx::Error error;
      bimg_ptr img = LoadKTXImage(matte_on_deck.path().c_str(), &error);

      _loader->StoreMatte(MatteFrameBimg {next, std::move (img)});
    }
}


}
