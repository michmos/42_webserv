#!/usr/bin/env python3

import sys
import os
import urllib.parse

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
		if self.request_method == "POST":
			return self.location_header("successupload.html")
		elif self.request_method == "DELETE":
			return self.location_header("successdelete.html")
		elif self.status_code != 200:
			return self.status_header()
		else:
			return self.cgi_header()
	
	# header when everything went good
	def cgi_header(self) -> str:
		header = "HTTP/1.1 " + http_status_messages[self.status_code] + "\n" \
					"Content-Type: text/html\n\n"
		body = "Success\n\n"
		return header + body

	# header when something turned wrong
	def status_header(self) -> str:
		header = "HTTP/1.1 " + http_status_messages[self.status_code] + "\n" \
				"Content-Type: text/html\n\n"
		body = "<html><head><title>Invalid data</title></head>" \
				"<body><h1>Invalid data typed</h1>" \
				"<br><pre>The data entered must be valid numeric digits for id number" \
				"<br></pre></body></html>\n\n"
		return header + body
	
	# header with redirection
	def location_header( self,  location ) -> str:
		header = "HTTP/1.1 302 Found\n" \
				"Location: " + location + "\n" + \
				"Content-Type: text/html\n\n"
		return header

class DeleteFile:
	def __init__( self, filename ):
		self.file = "../www/uploads/" + filename
	
	def is_existing_file( self ) -> bool:
		if os.path.isfile(self.file):
			return True 
		else:
			return False
	
	def delete_file( self ) -> int:
		if self.is_existing_file():
			os.remove(self.file)
			return 200
		return 404

class UploadFile:
	def __init__(self, filename):
		self.file = filename
		self.path = ""
		
	def is_direction( self, path ) -> bool:
		return (os.path.exists(path))

	def create_directory( self, path ):
		current_dir = os.getcwd()
		try:
			os.makedirs(path, exist_ok=True)
			return
		except PermissionError:
			sys.stderr.write(f"Permission denied: Unable to create '{path}'.")
		except Exception as e:
			sys.stderr.write(f"An error occurred: {e}")
		write_to_stdout("Error 500")

	def prepare_path( self ):
		base_path = "../www/uploads/"
		if not self.is_direction(base_path):
			self.create_directory(base_path)
		self.path = os.path.join(base_path + self.file)
		return 200

	def write_to_file( self, filename, output ):
		try:
			with open(filename, "a") as f:
				f.write(output)
		except Exception as e:
			sys.stderr.write(f"Error writing to file: {e}")
			return 500
		return 200

	def upload_file( self, post_data ) -> int:
		status_code = self.prepare_path()
		if status_code != 200:
			return status_code
		return self.write_to_file(self.path, post_data)

class CGI:
	def __init__( self ):	
		self.env = {}
		self.request_target = os.getenv('REQUEST_TARGET', '')
		self.method = os.getenv('REQUEST_METHOD', 'GET')
		self.contenttype = os.getenv('CONTENT_TYPE', '')
		self.content_length = 0
		try:
			self.content_length = int(os.getenv('CONTENT_LENGTH', 0))
		except:
			self.content_length = 0
		self.status_code = 200
		self.post_data = ""
		
	def get_status_code( self ):
		return self.status_code
	
	def parse_stdin_data( self ):
		if self.content_length > 0:
			while len(self.post_data) < self.content_length:
				self.post_data += sys.stdin.read(min(self.content_length - len(self.post_data), 2024))
		if len(self.post_data) != self.content_length:
			self.status_code = 400

	def parse_query_string( self ):
		query_string = os.getenv('QUERY_STRING', 0)
		self.env = dict(urllib.parse.parse_qsl(query_string))

	def parsing_cgi_input( self ):
		self.parse_query_string()
		if self.method == "POST":
			self.parse_stdin_data()
		elif self.method != "DELETE":
			status_code = 400

	def process_request( self ):
		if self.status_code != 200:
			return
		if self.method == "POST":
			uploader = UploadFile(self.request_target)
			status_code = uploader.upload_file(self.post_data)
		elif self.method == "DELETE":
			deleter = DeleteFile(self.request_target)
			status_code = deleter.delete_file()
		
	def write_to_stdout( self, response ):
		sys.stdout.write(response)

	def generate_response( self ):
		response = ResponseGenerator(self.method, self.status_code)
		self.write_to_stdout(response.create_response())

# Need with env: filename / requestmethod / contentlength / querystring in UPPERCASE?
def main() -> int:
	my_cgi = CGI()
	try:
		my_cgi.parsing_cgi_input()
		my_cgi.process_request()
		my_cgi.generate_response()
	except Exception as e:
		sys.stderr.write(e)
		return (400)
	else:
		print("ready")
	return my_cgi.get_status_code()

if __name__ == '__main__':
	main()