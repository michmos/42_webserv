#!/usr/bin/env python3
import sys

http_status_messages = {
	100: "100 Continue", 101: "101 Switching Protocols", 102: "102 Processing",
	200: "200 OK", 201: "201 Created", 202: "202 Accepted", 204: "204 No Content",
	301: "301 Moved Permanently", 302: "302 Found", 304: "304 Not Modified",
	307: "307 Temporary Redirect", 308: "308 Permanent Redirect",
	400: "400 Bad Request", 401: "401 Unauthorized", 403: "403 Forbidden",
	404: "404 Not Found", 405: "405 Method Not Allowed", 408: "408 Request Timeout",
	409: "409 Conflict", 410: "410 Gone", 429: "429 Too Many Requests",
	500: "500 Internal Server Error", 501: "501 Not Implemented",
	502: "502 Bad Gateway", 503: "503 Service Unavailable",
	504: "504 Gateway Timeout", 505: "505 HTTP Version Not Supported"
}

class ResponseGenerator:
	def __init__(self, request_method, status_code):
		self.status_code = status_code
		self.request_method = request_method

	# choose right header
	def create_response( self ) -> str:
		print("code", self.status_code, "method", self.request_method, file=sys.stderr)
		if self.status_code != 200 and self.status_code != 302:
				return self.status_header()
		if self.request_method == "POST":
			return self.location_header("successupload.html")
		elif self.request_method == "DELETE":
			return self.location_header("successdelete.html")
		else:
			return self.cgi_header()
	
	# header when everything went good
	def cgi_header(self) -> str:
		body = "Success\n\n"
		header = (
			f"HTTP/1.1 {http_status_messages[self.status_code]}\r\n"
			f"Content-Type: text/html\r\n"
			f"Content-Length: {len(body)}\r\n"
			f"\r\n"
		)
		return header + body

	# header when something turned wrong
	def status_header(self) -> str:
		body = (
			f"<html><head><title>{http_status_messages[self.status_code]}</title></head>"
			"<body></body></html>\n\n"
		)
		header = (
			f"HTTP/1.1 {http_status_messages[self.status_code]}\r\n"
			f"Content-Type: text/html\r\n"
			f"Content-Length: {len(body)}\r\n"
			f"\r\n"
		)
		return header + body
	
	# header with redirection
	def location_header( self,  location ) -> str:
		header = (
			f"HTTP/1.1 302 Found\r\n"
			f"Location: {location}\r\n"
			f"Content-Type: text/html\r\n\r\n"
		)
		return header