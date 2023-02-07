#pragma once

// enum for client state
enum ClientState
{
	Disconnected,
	Connecting,
	Connected,
	Sending,
	Receiving,
	Disconnecting
};

// enum for server state
enum ServerState
{
	Listening,
	Connecting,
	Waiting,
	Receiving,
	Verifying,
	Sending
};
