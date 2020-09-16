#include "Node.hpp"

#include <Layer.hpp>
#include <Renderable.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>

namespace charm {

Node::Node ()
  : m_layer {nullptr},
    m_parent {nullptr}
{
}

Node::~Node ()
{
  for (Node *node : m_children)
    delete node;
  m_children.clear ();

  for (Renderable *rend : m_renderables)
    delete rend;
  m_renderables.clear ();

  m_parent = nullptr;
  m_layer = nullptr;
}

void Node::UpdateTransformations ()
{
  TransformationSoftValue tsv;
  UpdateTransformations (tsv);
}

static void update_transformations_internal (Transformation &_tx,
                                             u32 _order,
                                             glm::vec3 const &_translation,
                                             glm::quat const &_rotation,
                                             glm::vec3 const &_scale)
{
  /*
    000: TRS
    001: TSR
    002: STR
    003: SRT
    004: RTS
    004: RST
   */

  auto make_inverse_transpose = [] (glm::mat4 const &_model) -> glm::mat3
  {
    return glm::inverseTranspose (glm::mat3 (_model));
  };

  //the inconsistency here drives me bonkers
  switch (_order) {
  case 0x000:
     _tx.model = glm::translate (_translation) * glm::mat4_cast(_rotation) * glm::scale (_scale);
     _tx.normal = glm::inverseTranspose (glm::mat4_cast(_rotation) * glm::scale (_scale));
     break;

  case 0x001:
     _tx.model = glm::translate (_translation) * glm::scale (_scale) * glm::mat4_cast(_rotation);
     _tx.normal = make_inverse_transpose(_tx.model);
     break;

  case 0x002:
     _tx.model = glm::scale (_scale) * glm::translate (_translation) * glm::mat4_cast(_rotation);
     _tx.normal = make_inverse_transpose (_tx.model);
     break;

  case 0x003:
     _tx.model = glm::scale (_scale) * glm::mat4_cast(_rotation) * glm::translate (_translation);
     _tx.normal = make_inverse_transpose (_tx.model);
     break;

  case 0x004:
     _tx.model = glm::mat4_cast(_rotation) * glm::translate (_translation) * glm::scale (_scale);
     _tx.normal = make_inverse_transpose (_tx.model);
     break;

  case 0x005:
     _tx.model = glm::mat4_cast(_rotation) * glm::scale (_scale) * glm::translate (_translation);
     _tx.normal = make_inverse_transpose (_tx.model);
     break;

  default:
     _tx.model = glm::translate (_translation) * glm::mat4_cast(_rotation) * glm::scale (_scale);
     _tx.normal = glm::inverseTranspose (glm::mat4_cast(_rotation) * glm::scale (_scale));
     break;
  }
}

void Node::UpdateTransformations (TransformationSoftValue const &_parent_tx)
{
  m_absolute_tx.ClearDirty ();

  if (m_translation.IsDirty() ||
      m_scale.IsDirty()       ||
      m_rotation.IsDirty())
    {
      m_translation.ClearDirty();
      m_scale.ClearDirty();
      m_rotation.ClearDirty();
      m_tx.SetDirty(true);

      Transformation &tx = m_tx.GetValue ();
      update_transformations_internal(tx, 0, m_translation.GetValue(),
                                      m_rotation.GetValue(),
                                      m_scale.GetValue());
    }

  if (m_tx.IsDirty() || _parent_tx.IsDirty ())
    {
      Transformation &abs_tx = m_absolute_tx.GetValue ();
      abs_tx.model = _parent_tx.GetValue ().model * m_tx.GetValue ().model;
      abs_tx.normal = _parent_tx.GetValue ().normal * m_tx.GetValue ().normal;
      m_absolute_tx.SetDirty(true);
      m_tx.ClearDirty ();
    }

  size_t const child_count = m_children.size ();
  for (size_t i = 0u; i < child_count; ++i)
    m_children[i]->UpdateTransformations (m_absolute_tx);
}

void Node::EnumerateRenderables()
{
  graph_id id = 0u;
  EnumerateRenderables(id);
}

void Node::EnumerateRenderables (graph_id &_id)
{
  size_t const rend_count = m_renderables.size ();
  for (size_t i = 0; i < rend_count; ++i)
    m_renderables[i]->m_graph_id = _id++;

  size_t const child_count = m_children.size ();
  for (size_t i = 0; i < child_count; ++i)
    m_children[i]->EnumerateRenderables (_id);
}

//node takes ownership of child nodes
void Node::AppendChild (Node *_node)
{
  if (_node->m_parent)
    _node->m_parent->ExciseChild (_node);

  _node->m_parent = this;
  _node->SetLayer (m_layer);

  m_children.push_back (_node);
}

void Node::RemoveChild (Node *_node)
{
  if (ExciseChild (_node))
    delete _node;
}

Node *Node::ExciseChild (Node *_node)
{
  auto it = std::find (m_children.begin (), m_children.end (), _node);
  if (it == m_children.end ())
    return nullptr;

  Node *n = *it;
  m_children.erase (it);
  n->SetLayer (nullptr);
  return n;
}

void Node::AppendRenderable (Renderable *_render)
{
  if (! _render)
    return;

  m_renderables.push_back (_render);
  _render->m_node = this;

  if (m_layer)
    m_layer->GetRenderables().push_back(_render);
}

void Node::RemoveRenderable (Renderable *_render)
{
  if (ExciseRenderable(_render))
    delete _render;
}

Renderable *Node::ExciseRenderable (Renderable *_render)
{
  auto it = std::find (m_renderables.begin (), m_renderables.end (), _render);
  if (it == m_renderables.end ())
    return nullptr;

  Renderable *r = *it;
  m_renderables.erase (it);
  if (m_layer)
    m_layer->RemoveRenderable (r);

  return r;
}

void Node::SetLayer (Layer *_layer)
{
  if (m_layer)
    m_layer->RemoveRenderables(m_renderables);

  m_layer = _layer;
  if (m_layer)
    m_layer->m_renderables.insert(m_layer->m_renderables.end (),
                                  m_renderables.begin (),
                                  m_renderables.end ());

  for (Node *child : m_children)
    child->SetLayer (_layer);
}

Layer *Node::GetLayer () const
{
  return m_layer;
}

VecAnim &Node::GetTranslationSoft ()
{
  return m_translation;
}

glm::vec3 const &Node::GetTranslation () const
{
  return m_translation.GetValue();
}

void Node::InstallTranslationAnimation (SoftAnimation<VecAnim> *_anim)
{
  m_translation.SetAnimation(_anim);

  if (_anim)
    {
      _anim->SetSoftValue(&m_translation);
      m_translation.SetDirty(true);
      AnimationSystem::GetSystem()->AddAnimation(_anim);
    }
}

QuatAnim &Node::GetRotationSoft ()
{
  return m_rotation;
}

glm::quat const &Node::GetRotation () const
{
  return m_rotation.GetValue();
}

void Node::InstallRotationAnimation (SoftAnimation<QuatAnim> *_anim)
{
  m_rotation.SetAnimation(_anim);

  if (_anim)
    {
      _anim->SetSoftValue(&m_rotation);
      m_rotation.SetDirty(true);
      AnimationSystem::GetSystem()->AddAnimation(_anim);
    }
}

VecAnim &Node::GetScaleSoft ()
{
  return m_scale;
}

glm::vec3 const &Node::GetScale () const
{
  return m_scale.GetValue();
}

void Node::InstallScaleAnimation (SoftAnimation<VecAnim> *_anim)
{
  m_scale.SetAnimation(_anim);

  if (_anim)
    {
      _anim->SetSoftValue(&m_scale);
      m_scale.SetDirty(true);
      AnimationSystem::GetSystem()->AddAnimation(_anim);
    }
}

TransformationSoftValue &Node::GetAbsoluteTransformationSoft ()
{
  return m_absolute_tx;
}

glm::mat4 const &Node::GetAbsoluteModelTransformation () const
{
  return m_absolute_tx.GetModel();
}

glm::mat4 const &Node::GetAbsoluteNormalTransformation () const
{
  return m_absolute_tx.GetNormal();
}

TransformationAnimSoftValue &Node::GetTransformationSoft ()
{
  return m_tx;
}

glm::mat4 const &Node::GetModelTransformation () const
{
  return m_tx.GetModel();
}

glm::mat4 const &Node::GetNormalTransformation () const
{
  return m_tx.GetNormal();
}

void Node::InstallTransformAnimation (TransformationAnimation *_animation)
{
  m_tx.SetAnimation(_animation);

  if (_animation)
    {
      _animation->SetSoftValue(&m_tx);
      m_tx.SetDirty(true);
      AnimationSystem::GetSystem()->AddAnimation(_animation);
    }
}

Animation *Node::GetTransformationAnimation () const
{
  return m_tx.GetAnimation();
}

}
