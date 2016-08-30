#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "20";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.06";
	
	//Software Status
	static const char STATUS[] =  "Beta";
	static const char STATUS_SHORT[] =  "b";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 0;
	static const long BUILD  = 4;
	static const long REVISION  = 21;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 143;
	#define RC_FILEVERSION 1,0,4,21
	#define RC_FILEVERSION_STRING "1, 0, 4, 21\0"
	static const char FULLVERSION_STRING [] = "1.0.4.21";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 4;
	

}
#endif //VERSION_H
