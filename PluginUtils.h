#ifndef PLUGINUTILS_H
#define PLUGINUTILS_H

#define CHECK_PROP_PROVIDED(PROP, STOR) \
	do { \
		if(!dataSet->get(PROP, STOR)) \
			throw std::runtime_error(std::string("No \"") + PROP + "\" provided"); \
	} while(0)

#endif /* PLUGINUTILS_H */
