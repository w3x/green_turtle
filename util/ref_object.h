//Copyright 2012, egmkang wang.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// auhor: egmkang (egmkang@gmail.com)

#ifndef __REF_OBJECT_H__
#define __REF_OBJECT_H__
#include <cstddef>
#include <type_traits>

namespace green_turtle{namespace util{

//a smart pointer just like std::weak_ptr
class RefObject;

namespace details{
  class RefCountImpl
  {
   public:
    RefCountImpl(RefObject *ptr)
    {
      host_ptr_ = ptr;
      ref_count_ = 1;
    }
    ~RefCountImpl()
    {
    }
    void AddRefCount()
    {
      ++ref_count_;
    }
    void SubRefCount()
    {
      --ref_count_;
      if(!ref_count_)
      {
        delete this;
      }
    }
    void Set(RefObject *ptr)
    {
      host_ptr_ = ptr;
    }
    RefObject* Get() const
    {
      return host_ptr_;
    }
   private:
    RefObject *host_ptr_;
    size_t    ref_count_;
  };
};

class RefObject{
 public:
  RefObject():impl_ptr_(NULL)
  {
    impl_ptr_ = new details::RefCountImpl(this);
  }
  virtual ~RefObject()
  {
    impl_ptr_->Set(NULL);
    impl_ptr_->SubRefCount();
  }
  details::RefCountImpl* GetRefCountImpl() const
  {
    return impl_ptr_;
  }
 private:
  details::RefCountImpl *impl_ptr_;
};
template<class T>
class RefPtr
{
 public:
  RefPtr():impl_ptr_(NULL){}
  ~RefPtr()
  {
    if(impl_ptr_)
    {
      impl_ptr_->SubRefCount();
    }
  }
  RefPtr(RefObject *ref_obj)
  {
    impl_ptr_ = ref_obj->GetRefCountImpl();
    impl_ptr_->AddRefCount();
  }
  RefPtr(const RefObject *ref_obj)
  {
    static_assert(std::is_const<T>::value,"const RefObjct* cannot be converted to RefPtr<T>,T is non const");
    impl_ptr_ = ref_obj->GetRefCountImpl();
    impl_ptr_->AddRefCount();
  }
  RefPtr(const RefPtr& ref_ptr)
  {
    impl_ptr_ = ref_ptr.impl_ptr_;
    impl_ptr_->AddRefCount();
  }
  RefPtr<T>& operator = (const RefPtr& ref_ptr)
  {
    if(impl_ptr_ == ref_ptr.impl_ptr)
      return *this;
    if(impl_ptr_)
    {
      impl_ptr_->SubRefCount();
    }
    impl_ptr_ = ref_ptr.impl_ptr_;
    if(impl_ptr_)
    {
      impl_ptr_->AddRefCount();
    }
    return *this;
  }
  T* Get() const
  {
    return impl_ptr_ ? (static_cast<T*>(impl_ptr_->Get())) : NULL;
  }

  T* operator->() const
  {
    return Get();
  }
  T& operator*() const
  {
    return *Get();
  }
  operator T*() const
  {
    return Get();
  }
  operator bool() const
  {
    return Get();
  }
 private:
  details::RefCountImpl *impl_ptr_;
};

};
};

#endif
