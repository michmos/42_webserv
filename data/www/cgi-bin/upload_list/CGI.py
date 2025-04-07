#!/usr/bin/env python3

import sys
import os
import html
import select

class CGI:
	def __init__( self ):
		self.status_code = 200
		self.header = ""
		self.body = ""
		
	def get_status_code( self ) -> int:
		return self.status_code

	def parsing( self ) -> None:
		# try:
		# 	data = sys.stdin.read()
		# 	print(f"Received data: {data}")
		# except Exception as e:
		# 	print(f"Error/No body in read from stdin in CGI : {e}\n", file=sys.stderr)
		
		if "REQUEST_METHOD" in os.environ:
			if os.getenv('REQUEST_METHOD') != "GET":
				print("Error: wrong request method", file=sys.stderr)
				self.status_code = 400
				return
		else:
			print("Error: no request method", file=sys.stderr)
			self.status_code = 400
			return

	def process_request( self ) -> None:
		body1 = "<!DOCTYPE html><html><body><h1>Uploaded files:</h1><ul>"
		body2 = "</ul><br><a href=\"index.html\" class=\"button\">Go back</a></body></html>"
		upload_dir = "data/uploads"

		files = os.listdir(upload_dir)
		if (len(files) == 0):
			body1 += "<p>No files uploaded</p>"
		else:
			for file in files:
				safe_file = html.escape(file)
				body1 += f"<li>{safe_file}<a href=\"nph_CGI_delete.py?file={safe_file}\" onclick=\"return confirm('Are you sure?');\"> [DELETE]</a></li>"

		self.body = body1 + body2
		self.header = "HTTP/1.1 200 OK\r\n" \
			+ "Content-Type: text/html\r\n" \
			+ "Content-Length: " + str(len(self.body)) + "\r\n\r\n"
	
	def write_to_stdout( self, response:str ) -> None:
		sys.stdout.write(response)
		sys.stdout.flush()

	def generate_response( self ) -> None:
		self.process_request()
		self.write_to_stdout(self.header + self.body)
		sys.stdout.close()
