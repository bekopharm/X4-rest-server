#include "HttpServer.h"
#include "../ffi/FFIInvoke.h"

HttpServer::HttpServer(FFIInvoke& ffi_invoke) : ffi_invoke_(ffi_invoke)
{
}

void HttpServer::run(int port)
{
	server.Get("/player-name", [&](const httplib::Request& req, httplib::Response& res) {
		res.set_content(
			ffi_invoke_.GetPlayerName().dump(), "text/json");
		});
	server.Get("/soft-target", [&](const httplib::Request& req, httplib::Response& res) {
		res.set_content(
			ffi_invoke_.GetSofttarget().dump(), "text/json");
		});

	server.Get("/component-details", [&](const httplib::Request& req, httplib::Response& res) {
		std::string compId = req.get_param_value("componentId");
		std::string connectionName = req.get_param_value("connectionName");
		if (compId.empty())
		{
			return BadRequest(res, "componentId must be set");
		}
		if (connectionName.empty())
		{
			connectionName = "X4RestServer";
			//return BadRequest(res, "connectionName must be set");
		}
		try {
			X4FFI::UniverseID uniId = std::stoll(compId);
			res.set_content(
				ffi_invoke_.GetComponentDetails(uniId, connectionName.c_str()).dump(), "text/json");
		} catch (std::exception&) {
			return BadRequest(res, "componentId malformed");
		}
	});



	
	server.Get("/stop", [&](const httplib::Request& req, httplib::Response& res) {
		std::thread([&]() {
			server.stop();
			}).detach();
		// produces empty response
		});
	// somehow got bad request every other request without this o.O
	std::thread([&]()
	{
			server.listen("0.0.0.0", port);
	}).join();
}

void HttpServer::BadRequest(httplib::Response& res, std::string const& message)
{
	res.status = 401;
	res.set_content(json{
		{"code", 401},
		{"name", "Bad Request"},
		{"message", message},
	}.dump(), "text/json");
}