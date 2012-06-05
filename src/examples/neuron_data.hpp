#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class neuron_data
{
private:
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & a;
		ar & b;
		ar & c;
		ar & d;
		ar & u;
		ar & v;
		ar & sigma;
		ar & nidx;
	}
	float a;
	float b;
	float c;
	float d;
	float u;
	float v;
	float sigma;
	unsigned nidx;
public:
	neuron_data(){};
	neuron_data(float a, float b, float c, float d, float u, float v, float sigma, unsigned nidx) :
        a(a), b(b), c(c), d(d), u(u), v(v), sigma(sigma), nidx(nidx) {}
};
