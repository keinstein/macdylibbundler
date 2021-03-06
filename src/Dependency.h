/*  -*- C++ -*-
The MIT License (MIT)

Copyright (c) 2014 Marianne Gagnon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 */


#ifndef _depend_h_
#define _depend_h_

#include <string>
#include <map>
#include <set>

class Dependency;
typedef std::set<Dependency> DependencyMap;


class DependencyKey {
public:
	DependencyKey() {}
	DependencyKey(const std::string & f) {
		size_t pos = f.find_last_of("/");
		if (pos != std::string::npos) {
			prefix = f.substr(0,pos+1);
			filename = f.substr(pos+1,std::string::npos);
		}
	}
	DependencyKey(const std::string & p,
		      const std::string & f): prefix(p),
					      filename(f) {}
	DependencyKey(const DependencyKey & o):prefix(o.prefix),
					  filename(o.filename)
	{}
	DependencyKey(const DependencyKey && o):prefix(o.prefix),
					  filename(o.filename)
	{}

	std::string getOriginalFileName() const{ return filename; }
	std::string getOriginalPath() const{ return prefix+filename; }
	std::string getPrefix() const{ return prefix; }

	bool operator < (const DependencyKey & o) const {
		return (filename < o.filename || (filename == o.filename  && prefix < o.prefix));
	}
protected:
	std::string prefix;
	std::string filename;
};

class Dependency: public DependencyKey
{
 public:
    typedef std::multimap<std::string,std::string> filenamelist;
 protected:
    // origin
    filenamelist dependents;

    // installation
    std::string new_name;
public:
    Dependency(const DependencyKey & o):DependencyKey(o) {}
    Dependency(const DependencyKey && o):DependencyKey(o) {}

    Dependency(std::string path,
	       const std::string & dependent);

    void print() const;

    std::string getInstallPath() const;
    std::string getInnerPath() const;

    void addSymlink(const std::string & symlink,
		    const std::string & dependent) {
	    dependents.insert(std::pair<std::string,std::string>(symlink,dependent));
    }
    filenamelist::const_iterator dependentsBegin() { return dependents.begin(); }
    filenamelist::const_iterator dependentsEnd() { return dependents.end(); }

    void copyYourself() const;
    void fixFileThatDependsOnMe(const Dependency & file,
				const std::string & symlink) const;
    void fixFilesThatDependOnMe(DependencyMap& deps) const;
    void merge(const Dependency& dep2) {
	dependents.insert(dep2.dependents.begin(),
			  dep2.dependents.end());
    }

    using DependencyKey::operator<;
};


#endif
