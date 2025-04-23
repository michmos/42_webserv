#!/usr/bin/env python3

import sys
import os

http_status_messages = {
	100: "100 Continue", 101: "101 Switching Protocols", 102: "102 Processing",
	200: "200 OK", 201: "201 Created", 202: "202 Accepted", 204: "204 No Content",
	301: "301 Moved Permanently", 302: "302 Found", 304: "304 Not Modified",
	307: "307 Temporary Redirect", 308: "308 Permanent Redirect",
	400: "400 Bad Request", 401: "401 Unauthorized", 403: "403 Forbidden",
	404: "404 Not Found", 405: "405 Method Not Allowed", 408: "408 Request Timeout",
	409: "409 Conflict", 410: "410 Gone", 415: "415 Unsupported Media Type", 429: "429 Too Many Requests",
	500: "500 Internal Server Error", 501: "501 Not Implemented",
	502: "502 Bad Gateway", 503: "503 Service Unavailable",
	504: "504 Gateway Timeout", 505: "505 HTTP Version Not Supported"
}

good_status_codes = [100, 200, 201, 302]

class ResponseGenerator:
	def __init__( self, request_method:str, status_code:int ):
		self.status_code = status_code
		self.request_method = request_method


	def nph_headers( self ) -> str:
		if self.status_code not in good_status_codes:
			return self.nph_error_status_code_header()
		elif self.request_method == "POST":
			return self.nph_redirection_header("../http/successupload.html")
		elif self.request_method == "DELETE":
			return self.nph_redirection_header("../http/successdelete.html")
		else:
			return self.nph_success_header()

	# choose right header
	def create_response( self ) -> str:
		script_name = os.path.basename(sys.argv[0])
		if (script_name.startswith("nph_")):
			return self.nph_headers()
		else:
			if self.status_code in good_status_codes:
				if self.request_method == "POST":
					return self.local_location("../http/successupload.html")
				elif script_name == "delete.py":
					return self.local_location("../http/succesdelete.html")
				else:
					return document_response()
			else:
				return document_response()
	
# NPH headers:
	# header when everything went good
	def nph_success_header( self ) -> str:
		body = "Success\n\n"
		header = (
			f"HTTP/1.1 {http_status_messages[self.status_code]}\r\n"
			f"Content-Type: text/html\r\n"
			f"Content-Length: {len(body)}\r\n"
			f"\r\n"
		)
		return header + body	

	# header when something turned wrong
	def nph_error_status_code_header( self ) -> str:
		body = (
			f"<!DOCTYPE html><html><head><title>{http_status_messages[self.status_code]}</title></head>"
			f"<body><p>{http_status_messages[self.status_code]}</p></body></html>\r\n"
		)
		header = (
			f"HTTP/1.1 {http_status_messages[self.status_code]}\r\n"
			f"Content-Type: text/html\r\n"
			f"Content-Length: {len(body)}\r\n"
			f"\r\n"
		)
		return header + body
	
	# header with redirection
	def nph_redirection_header( self,  location:str ) -> str:
		header = (
			f"HTTP/1.1 302 Found\r\n"
			f"Location: {location}\r\n"
			f"Content-Type: text/html\r\n\r\n"
		)
		return header
	
# Headers if not nph so server have to check and rewrite response for client:
	def document_response( self ) -> str:
		content_type = "Content-Type: text/plain\r\n"
		status = f"Status {http_status_messages[self.status_code]}\r\n"
		other_fields = ""
		newline = "\r\n"
		response_body = ""

		return content_type + status + other_fields + newline + response_body
	
	def local_redir_reponse( self, local_pathquery:str ) -> str:
		local_location = f"Location: {local_pathquery}\r\n"
		newline = "\r\n"

		return local_location + newline
	
	def client_redir_response( self, fragment_URI: str ) -> str:
		client_location = f"Location: {fragment_URI}\r\n"
		newline = "\r\n"

		return client_location + newline

	def client_redirdoc_response( self, fragment_URI:str ) -> str:
		client_location = f"Location: {fragment_URI}\r\n"
		status = f"Status {http_status_messages[self.status_code]}\r\n"
		content_type = "Content-Type: text/plain\r\n"
		other_fields = ""
		newline = "\r\n"
		response_body = ""
