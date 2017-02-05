// From: http://sourceforge.net/p/poco/feature-requests/168/
//
// RecursiveDirectoryIterator.h
//
// $Id$
//
// Library: Foundation
// Package: Filesystem
// Module:  RecursiveDirectoryIterator
//
// Definition of the RecursiveDirectoryIterator class.
//
// Copyright (c) 2012, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//


#ifndef Foundation_RecursiveDirectoryIterator_INCLUDE
#define Foundation_RecursiveDirectoryIterator_INCLUDE


#include "Poco/Foundation.h"
#include "Poco/File.h"
#include "Poco/Path.h"
#include "ofx/RecursiveDirectoryIteratorImpl.h"
#include "ofx/RecursiveDirectoryIteratorStategies.h"


namespace ofx {


    class DirectoryIterator;

    template <class TTravStr>
    class RecursiveDirectoryIteratorImpl;


    template <class TTravStr = ChildrenFirstTraverse>
    class RecursiveDirectoryIterator
	/// The RecursiveDirectoryIterator class is used to enumerate
	/// all files in a directory and its subdirectories.
	///
	/// RecursiveDirectoryIterator has some limitations:
	///   * only forward iteration (++) is supported
	///   * an iterator copied from another one will always
	///     point to the same file as the original iterator,
	///     even is the original iterator has been advanced
	///     (all copies of an iterator share their state with
	///     the original iterator)
	///
	/// The class can follow different traversal strategies:
	///   * depth-first strategy;
	///   * siblings-first strategy.
	/// The stategies are set by template parameter.
	/// There are two corresponding typedefs:
	///   * SimpleRecursiveDirectoryIterator;
	///   * SiblingsFirstRecursiveDirectoryIterator.
	///
	/// The depth of traversal can be limited by constructor
	/// parameter maxDepth (which sets the infinite depth by default).
    {
    public:
        typedef RecursiveDirectoryIterator<TTravStr> MyType;

        enum { D_INFINITE = 0 };
        /// Constant for infinite traverse depth.

        RecursiveDirectoryIterator();
		/// Creates the end iterator.

        RecursiveDirectoryIterator(const std::string& path,
                                   Poco::UInt16 maxDepth = D_INFINITE);
		/// Creates a recursive directory iterator for the given path.

        RecursiveDirectoryIterator(const MyType& iterator);
		/// Creates a copy of another recursive directory iterator.

        RecursiveDirectoryIterator(const Poco::DirectoryIterator& iterator,
                                   Poco::UInt16 maxDepth = D_INFINITE);
		/// Creates a recursive directory iterator for the path of
		/// non-recursive directory iterator.

        RecursiveDirectoryIterator(const Poco::File& file,
                                   Poco::UInt16 maxDepth = D_INFINITE);
		/// Creates a recursive directory iterator for the given path.

        RecursiveDirectoryIterator(const Poco::Path& path,
                                   Poco::UInt16 maxDepth = D_INFINITE);
		/// Creates a recursive directory iterator for the given path.

        ~RecursiveDirectoryIterator();
		/// Destroys the DirectoryIterator.

        const std::string& name() const;
		/// Returns the current filename.

        const Poco::Path& path() const;
		/// Returns the current path.

        Poco::UInt16 depth() const;
		/// Depth of recursion (counting from 1).

        Poco::UInt16 maxDepth() const;
		/// Max depth of recursion (counting from 1).

        MyType& operator = (const MyType& it);
        MyType& operator = (const Poco::File& file);
        MyType& operator = (const Poco::Path& path);
        MyType& operator = (const std::string& path);

        MyType& operator ++ ();

        const Poco::File& operator * () const;
        Poco::File& operator * ();
        const Poco::File* operator -> () const;
        Poco::File* operator -> ();

        template <class T1, class T2>
        friend inline bool operator == (const RecursiveDirectoryIterator<T1>& a,
                                        const RecursiveDirectoryIterator<T2>& b);
        template <class T1, class T2>
        friend inline bool operator != (const RecursiveDirectoryIterator<T1>& a,
                                        const RecursiveDirectoryIterator<T2>& b);

    private:
        typedef RecursiveDirectoryIteratorImpl<TTravStr> ImplType;

        ImplType* _pImpl;
        Poco::Path _path;
        Poco::File _file;
    };


    //
    // friend comparsion operators
    //
    template <class T1, class T2>
    inline bool
    operator == (const RecursiveDirectoryIterator<T1>& a,
                 const RecursiveDirectoryIterator<T2>& b)
    {
        return a.path().toString() == b.path().toString();;
    }


    template <class T1, class T2>
    inline bool
    operator != (const RecursiveDirectoryIterator<T1>& a,
                 const RecursiveDirectoryIterator<T2>& b)
    {
        return a.path().toString() != b.path().toString();;
    }


    //
    // inlines
    //
    template <class TTravStr>
    inline const std::string&
    RecursiveDirectoryIterator<TTravStr>
    ::name() const
    {
        return _path.getFileName();
    }


    template <class TTravStr>
    inline const Poco::Path&
    RecursiveDirectoryIterator<TTravStr>
    ::path() const
    {
        return _path;
    }


    template <class TTravStr>
    inline Poco::UInt16
    RecursiveDirectoryIterator<TTravStr>
    ::depth() const
    {
        return _pImpl->depth();
    }


    template <class TTravStr>
    inline Poco::UInt16
    RecursiveDirectoryIterator<TTravStr>
    ::maxDepth() const
    {
        return _pImpl->maxDepth();
    }


    template <class TTravStr>
    inline const Poco::File&
    RecursiveDirectoryIterator<TTravStr>
    ::operator * () const
    {
        return _file;
    }


    template <class TTravStr>
    inline Poco::File&
    RecursiveDirectoryIterator<TTravStr>
    ::operator * ()
    {
        return _file;
    }


    template <class TTravStr>
    inline const Poco::File*
    RecursiveDirectoryIterator<TTravStr>
    ::operator -> () const
    {
        return &_file;
    }


    template <class TTravStr>
    inline Poco::File*
    RecursiveDirectoryIterator<TTravStr>
    ::operator -> ()
    {
        return &_file;
    }


    //
    // not inlines
    //
    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>
    ::RecursiveDirectoryIterator()
	: _pImpl(0)
    {
    }


    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>
    ::RecursiveDirectoryIterator(const std::string& path, Poco::UInt16 maxDepth)
	: _pImpl(new ImplType(path, maxDepth)),
	_path(Poco::Path(_pImpl->get())),
	_file(_path)
    {
    }


    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>
    ::RecursiveDirectoryIterator(const MyType& iterator)
	: _pImpl(iterator._pImpl),
	_path(iterator._path),
	_file(iterator._file)
    {
    }


    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>
    ::RecursiveDirectoryIterator(const Poco::DirectoryIterator& iterator, Poco::UInt16 maxDepth)
	: _pImpl(new ImplType(iterator->path(), maxDepth)),
	_path(Poco::Path(_pImpl->get())),
	_file(_path)
    {
    }


    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>
    ::RecursiveDirectoryIterator(const Poco::File& file, Poco::UInt16 maxDepth)
	: _pImpl(new ImplType(file.path(), maxDepth)),
	_path(Poco::Path(_pImpl->get())),
	_file(_path)
    {
    }


    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>
    ::RecursiveDirectoryIterator(const Poco::Path& path, Poco::UInt16 maxDepth)
	: _pImpl(new ImplType(path.toString(), maxDepth)),
	_path(Poco::Path(_pImpl->get())),
	_file(_path)
    {
    }


    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>
    ::~RecursiveDirectoryIterator()
    {
        if (_pImpl)
            _pImpl->release();
    }


    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>&
    RecursiveDirectoryIterator<TTravStr>
    ::operator = (const MyType& it)
    {
        if (_pImpl)
            _pImpl->release();
        _pImpl = it._pImpl;
        if (_pImpl)
        {
            _pImpl->duplicate();
            _path = it._path;
            _file = _path;
        }
        return *this;
    }
    
    
    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>&
    RecursiveDirectoryIterator<TTravStr>
    ::operator = (const Poco::File& file)
    {
        if (_pImpl)
            _pImpl->release();
        _pImpl = new ImplType(file.path());
        _path = Poco::Path(_pImpl->get());
        _file = _path;
        return *this;
    }
    
    
    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>&
    RecursiveDirectoryIterator<TTravStr>
    ::operator = (const Poco::Path& path)
    {
        if (_pImpl)
            _pImpl->release();
        _pImpl = new ImplType(path.toString());
        _path = Poco::Path(_pImpl->get());
        _file = _path;
        return *this;
    }
    
    
    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>&
    RecursiveDirectoryIterator<TTravStr>
    ::operator = (const std::string& path)
    {
        if (_pImpl)
            _pImpl->release();
        _pImpl = new ImplType(path);
        _path = Poco::Path(_pImpl->get());
        _file = _path;
        return *this;
    }
    
    
    template <class TTravStr>
    RecursiveDirectoryIterator<TTravStr>&
    RecursiveDirectoryIterator<TTravStr>
    ::operator ++ ()
    {
        if (_pImpl)
        {
            _path = Poco::Path(_pImpl->next());
            _file = _path;
        }
        return *this;
    }
    
    
    //
    // exported instances
    //
    template class Foundation_API
	RecursiveDirectoryIterator<ChildrenFirstTraverse>;
    template class Foundation_API
	RecursiveDirectoryIterator<SiblingsFirstTraverse>;
    
    
    //
    // typedefs
    //
    typedef RecursiveDirectoryIterator<ChildrenFirstTraverse>
	SimpleRecursiveDirectoryIterator;
    typedef RecursiveDirectoryIterator<SiblingsFirstTraverse>
	SiblingsFirstRecursiveDirectoryIterator;
    
    
} // namespace ofx


#endif // Foundation_RecursiveDirectoryIterator_INCLUDE