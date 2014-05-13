#include "Client.h"

class UDPClient: public Client
{
	public:
		UDPClient();
		virtual ~UDPClient();

	private:
		virtual std::string send_message(Message msg);

};
