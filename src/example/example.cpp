#include "ffi_client.hpp"

int main()
{
    using namespace LiveKit;

    FFIClient client;
    client.Initialize();
    client.Shutdown();

    return 0;
}