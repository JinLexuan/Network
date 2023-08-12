#pragma once

namespace np
{
namespace Endpoint
{
    int createClientEndpoint();
    int createServerEndpoint();
    int createTCPSocket();
    int createAcceptorSocket();
    int bindAcceptorSocket();
    int connectToEnd();
    int connectDNStoEnd();
    int acceptNewConnection();
} // namespace Endpoint
} // namespace np
