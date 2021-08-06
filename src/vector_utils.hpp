
#ifndef VECTOR_CAN_BE_TERRIBLY_ANNOYING
#define VECTOR_CAN_BE_TERRIBLY_ANNOYING


#include <base_types.hpp>

#include <algorithm>
#include <vector>


namespace zeugma  {


//Searches for element _t
//Returns the first index, -1 if not found
template<typename T>
i64 Find (std::vector<T> const &_vector, T const &_t)
{
  auto it = std::find (_vector.begin (), _vector.end (), _t);
  if (it == _vector.end ())
    return -1;

  return std::distance (_vector.begin (), it);
}

//Returns the first index that satisfies Pred, -1 otherwise
template<typename T, typename Pred>
i64 FindIf (std::vector<T> const &_vector, Pred _p)
{
  auto it = std::find_if (_vector.begin (), _vector.end (), _p);
  if (it == _vector.end ())
    return -1;

  return std::distance (_vector.begin (), it);
}

//Searches for _t and erases the first instance
//Returns true if found, false otherwise
template<typename T>
bool FindErase (std::vector<T> &_vector, T const &_t)
{
  auto it = std::find (_vector.begin (), _vector.end (), _t);
  if (it == _vector.end ())
    return false;

  _vector.erase (it);
  return true;
}

//Searches for the first element that satisfies Pred and erases it
//Returns true if found, false otherwise
template<typename T, typename Pred>
bool FindIfErase (std::vector<T> &_vector, Pred _p)
{
  auto it = std::find_if (_vector.begin (), _vector.end (), _p);
  if (it == _vector.end ())
    return false;

  _vector.erase (it);
  return true;
}

//Searches for the first element that satisfies Pred, moves it out,
//and erases the empty husk from the vector
//Returns true if found, false otherwise
template<typename T, typename Pred>
bool FindIfMoveErase (std::vector<T> &_vector, Pred _p, T &_out_t)
{
  auto it = std::find_if (_vector.begin (), _vector.end (), _p);
  if (it == _vector.end ())
    return false;

  _out_t = std::move (*it);
  _vector.erase (it);
  return true;
}

//Searches for the first element that satisfies Pred, copies it out,
//and erases the empty husk from the vector
//Returns true if found, false otherwise
template<typename T, typename Pred>
bool FindIfCopyErase (std::vector<T> &_vector, Pred _p, T &_out_t)
{
  auto it = std::find_if (_vector.begin (), _vector.end (), _p);
  if (it == _vector.end ())
    return false;

  _out_t = *it;
  _vector.erase (it);
  return true;
}

//Removes then erases from the vector every element _t
//Returns the number of erased elements
template<typename T>
i64 RemoveErase (std::vector<T> &_vector, T const &_t)
{
  auto it = std::remove (_vector.begin (), _vector.end (), _t);
  if (it == _vector.end ())
    return false;

  i64 const ret = std::distance (it, _vector.end ());
  _vector.erase (it, _vector.end ());
  return ret;
}

//Removes then erases from the vector every element that satisfies Pred
//Returns the number of erased elements
template<typename T, typename Pred>
i64 RemoveIfErase (std::vector<T> &_vector, Pred _p)
{
  auto it = std::remove_if (_vector.begin (), _vector.end (), _p);
  if (it == _vector.end ())
    return false;

  i64 const ret = std::distance (it, _vector.end ());
  _vector.erase (it, _vector.end ());
  return ret;
}


}


#endif  //VECTOR_CAN_BE_TERRIBLY_ANNOYING
