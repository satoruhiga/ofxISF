#pragma once

#define OFX_ISF_BEGIN_NAMESPACE namespace ofx { namespace ISF {
#define OFX_ISF_END_NAMESPACE } }

OFX_ISF_BEGIN_NAMESPACE

template <typename T>
struct Ref_ : public ofPtr<T>
{
	Ref_() {}
	Ref_(T *t) : ofPtr<T>(t) {}
	Ref_(const Ref_<T>& o) : ofPtr<T>(o) {}
	Ref_(const ofPtr<T>& o) : ofPtr<T>(o) {}

	template <typename TT>
	Ref_<TT> cast() const { return dynamic_pointer_cast<TT>(*this); }
};

template <typename T>
struct Type2Int
{
	static unsigned int value()
	{
		static size_t m = 0;
		return (unsigned int)&m;
	}
};

struct PresistentBuffer
{
	string name;
	float width, height;
};

struct Pass
{
	string target;
	float width, height;
};

OFX_ISF_END_NAMESPACE

namespace ofxISF = ofx::ISF;