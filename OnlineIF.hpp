#ifndef ONLINEIF_HPP_
#define ONLINEIF_HPP_

#include "Forest.hpp"

class OnlineIF: public Forest {
	int windowSize;
	int updateCount;

	void genInitTreeStructures(doubleframe *dataset);
public:
	OnlineIF(){}
	OnlineIF(int _ntree, doubleframe* _df, int _nsample, int _maxheight,
			bool _rsample, int _windowSize);
	virtual ~OnlineIF() {}
	double instanceScore(double *inst);
	void update(double inst[]);
};

#endif /* ONLINEIF_HPP_ */
