/*
	ShadowIRC - A Mac OS IRC Client
	Copyright (C) 1996-2005 John Bafford
	dshadow@shadowirc.com
	http://www.shadowirc.com

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "IRCSFeatures.h"
#include "utils.h"

#define FEATURE(name, type, var, varFunc) {name, type, sizeof(_defaultFeatures.var), (void*)&_defaultFeatures.var, (void*)&_defaultFeatures.var - (void*)&_defaultFeatures, varFunc}

#define FEATUREINT(name, var) FEATURE(name, featureInt, var, NULL)
#define FEATURESTR(name, var) FEATURE(name, featureStr, var, NULL)
#define FEATUREBOOL(name, var) FEATURE(name, featureBool, var, NULL)
#define FEATURESPECIAL(name, var, varFunc) FEATURE(name, featureSpecial, var, varFunc)

#define FEATUREEND {NULL, 0, 0, NULL}

typedef enum featureType {
	featureBool,
	featureInt,
	featureStr,
	featureSpecial,
} featureType;

typedef enum featureAction {
	kFeatureSet,
	kFeatureDelete,
	kFeatureDispose,
} featureAction;

struct featureDesc {
	StringPtr name;
	const featureType type;
	const int storageSize;
	void* def;
	int offset;
	void (*specialVarFunc)(const struct featureDesc *kf, featureAction action, void* data);
};

static IRCSFeatures _defaultFeatures = {
	0, //topicLength
	0, //kickLength
	0, //awayLength
	9, //nickLength
	4, //numModes
	10, //maxChannels
	0, //maxBans
	
	"\p#&", //chanTypes
	
	0, //hasWallchops
};

static const struct featureDesc knownFeatures[] = {
	FEATUREINT("\pTOPICLEN", topicLength),
	FEATUREINT("\pKICKLEN", kickLength),
	FEATUREINT("\pAWAYLEN", awayLength),
	FEATUREINT("\pNICKLEN", nickLength),
	FEATUREINT("\pMODES", numModes),
	FEATUREINT("\pMAXCHANNELS", maxChannels),
	FEATUREINT("\pMAXBANS", maxBans),
	
	FEATURESTR("\pCHANTPES", chanTypes),
	
	FEATUREBOOL("\pWALLCHOPS", hasWallchops),
	
	FEATUREEND
};

void SFDestroy(IRCSFeaturesPtr sf)
{
	if(sf)
	{
		const struct featureDesc *kf = knownFeatures;
		
		//Step through and delete all of the featureStrs
		while(kf->name)
		{
			if(kf->type == featureStr)
				DisposePtr((char*)(((char*)sf) + kf->offset));
			else if(kf->type == featureSpecial)
				kf->specialVarFunc(kf, kFeatureDispose, NULL);
			
			kf++;
		}
		
		free(sf);
	}
}

IRCSFeaturesPtr SFNew(void)
{
	const struct featureDesc *kf = knownFeatures;
	IRCSFeaturesPtr sf = calloc(1, sizeof(IRCSFeatures));
	
	while(kf->name)
	{
		SFDeleteFeature(sf, kf->name);
		
		kf++;
	}
	
	return sf;
}

static const struct featureDesc* _SFFindFeature(StringPtr name)
{
	const struct featureDesc *kf = knownFeatures;
	
	while(kf->name)
	{
		if(pstrcmp(kf->name, name))
			return kf;
		
		kf++;
	}
	
	return NULL;
}

char SFSetFeature(IRCSFeaturesPtr sf, StringPtr name, void* data)
{
	const struct featureDesc *kf = _SFFindFeature(name);
	
	if(kf)
	{
		void* targetLoc = (((char*)sf) + kf->offset);
		
		if(kf->type == featureBool)
			*(char*)targetLoc = !!data;
		else if(kf->type == featureInt)
			*(int*)targetLoc = (int)data;
		else if(kf->type == featureStr)
		{
			if(*(StringPtr*)targetLoc)
				DisposePtr(*(StringPtr*)targetLoc);
			
			*(StringPtr*)targetLoc = NewPString((StringPtr)data);
		}
		else if(kf->type == featureSpecial)
			kf->specialVarFunc(kf, kFeatureSet, data);
		
		return true;
	}
	
	return false;
}

char SFDeleteFeature(IRCSFeaturesPtr sf, StringPtr name)
{
	//Get the feature from the _defaultFeatures, then set it.
	const struct featureDesc *kf = _SFFindFeature(name);
	
	if(kf)
	{
		void* data = NULL;
		void* srcLoc = kf->def;
		
		if(kf->type == featureBool)
			data = (void*)((int)*(char*)srcLoc);
		else if(kf->type == featureInt)
			data = (void*)*(int*)srcLoc;
		else if(kf->type == featureStr)
			data = (void*)*(StringPtr*)srcLoc;
		else if(kf->type == featureSpecial)
			kf->specialVarFunc(kf, kFeatureDelete, NULL);
		else
			return false;
		
		return SFSetFeature(sf, name, data);
	}
	
	return false;
}
