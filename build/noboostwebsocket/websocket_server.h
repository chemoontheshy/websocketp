#ifndef __WEBSOCKET_SERVER_H_
#define __WEBSOCKET_SERVER_H_

#include <iostream>
#include <memory>

namespace vsnc
{
	namespace vnet
	{
		struct Data
		{
			uint8_t* ptr;
			size_t	 len;
		};

		class Pubisher
		{
		public:
			Pubisher(const uint16_t port);
			~Pubisher();

			bool Pubish(const Data& data);
		};
	}
}


#endif __WEBSOCKET_SERVER_H_