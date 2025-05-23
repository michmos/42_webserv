#!/usr/bin/env python3

import os
import urllib.parse
import cgi
import sys
import re
import time

class Parser:
	def __init__( self ):
		self.env = {}
		self.status_code = 200
		self.body = ""
	
	def get_status_code( self ) -> int:
		return self.status_code
	
	def get_body( self ) -> str:
		return self.body

	def is_in_env( self, name:str ) -> bool:
		if name not in os.environ:
			return False
		return True

	# Get the needed env from the environ and saves it in a dictonary
	def extract_env( self ) -> None:
		if self.is_in_env("CONTENT_LENGTH"):
			self.env["content_length"] = int(os.getenv('CONTENT_LENGTH'))
		if self.is_in_env("REQUEST_TARGET"):
			self.env["request_target"] = os.getenv('REQUEST_TARGET')
		else:
			print("Error: Request target is missing", file=sys.stderr)
			self.status_code = 400
			return
		if "CONTENT_TYPE" in os.environ:
			self.env["content_type"] = os.getenv('CONTENT_TYPE')
		else:
			self.status_code = 400
			print("Error: Content-type is missing", file=sys.stderr)
			return
	
	# True if filename has right extension (Have to check it with conf?)
	def is_valid_extension( self ) -> bool:
		allowed_extensions = {"jpg", "jpeg", "png", "txt"}
		if self.env["filename"]:
			splitter = self.env["filename"].split(".")
			if splitter[-1].lower() not in allowed_extensions:
				return False
			else:
				return True
		print(f"Error: missing filename", file=sys.stderr)
		return False

	# Extract the information if a Query_string is given
	def parse_query_string( self ) -> None:
		if "QUERY_STRING" in os.environ:
			query_string = os.getenv('QUERY_STRING')
			self.env.update(dict(urllib.parse.parse_qsl(query_string)))

	# Extract the filename from the header and saves it as a key-value pair in the env
	def save_filename_in_env( self, header:bytes ) -> None:
		first_index = header.find(b"filename=") + len(b"filename=") + 1
		second_index = header.find(b"\"", first_index)
		if (first_index == -1 or second_index == -1):
			self.status_code = 400
			print("Error: missing filename", file=sys.stderr)
			return
		self.env["filename"] = header[first_index:second_index].decode()

	# Get the boundary from Content_type
	def get_boundary( self, content_type:str ) -> bytes | None :
		start = content_type.find("boundary=") + 9
		if start == -1:
			print("Error: missing boundary in content-type", file=sys.stderr)
			self.status_code = 400
			return None
		boundary = b"" + content_type[start:-1].encode()
		return boundary

	# Extract the information between the boundary value
	def extract_boundary_data(self, boundary: bytes, post_data: bytes) -> str | None:
		first_index = post_data.find(boundary)
		if first_index == -1:
			print("Error: boundary not found", file=sys.stderr)
			self.status_code = 400
			return None

		header_end = post_data.find(b"\r\n\r\n", first_index)
		if header_end == -1:
			print("Error: headers not terminated", file=sys.stderr)
			self.status_code = 400
			return None
		
		header = post_data[first_index + len(boundary):header_end].strip(b"\r\n")
		content_start = header_end + 4

		end_index = post_data.find(b"--" + boundary + b"--", content_start)
		if end_index == -1:
			end_index = post_data.find(boundary, content_start)
			if end_index == -1:
				end_index = len(post_data)

		content = post_data[content_start:end_index].strip(b"\r\n")

		if content.endswith(b"--"):
	   		content = content[:-2]
		return header, content

	# Returns True when Content Length is not set or <= 0
	def	no_content_length( self ) -> bool:
		if not self.env["content_length"] or self.env["content_length"] <= 0:
			print("Error: missing content length for stdin data", file=sys.stderr)
			self.status_code = 400
			return True
		return False

	# With POST request, reads the input from STDIN, extract header and body and checks filename
	def parse_stdin_data( self ) -> None:
		if self.no_content_length():
			return
		post_data = b""
		length = int(self.env.get("content_length", 0))
		while len(post_data) < length:
			chunk = sys.stdin.buffer.read(length - len(post_data))
			if not chunk:
				break
			post_data += chunk

		if "multipart/form-data" not in self.env["content_type"]:
			self.status_code = 400
			print("Error: wrong content-type, no multipart/form-data", file=sys.stderr)
			return

		boundary = self.get_boundary(self.env["content_type"])
		header_body = self.extract_boundary_data(boundary, post_data)

		if len(header_body) != 2:
			self.status_code = 400
			print("Error: input contains more than one body", file=sys.stderr)
			return

		header, self.body = header_body
		self.save_filename_in_env(header)
		if not self.is_valid_extension():
			print("Error: No valid extension file", file=sys.stderr)
			self.status_code = 415

	# check method and handles POST request
	# returns env from Server and Stdin
	def parsing_cgi_input( self ) -> dict | None:
		if not (self.is_in_env("REQUEST_METHOD") and "POST" == os.getenv('REQUEST_METHOD')):
			print("Error: missing request method", file=sys.stderr)
			self.status_code = 400
			return

		# POST or DELETE REQUEST
		self.extract_env()
		if self.status_code != 200:
			return
		self.parse_query_string()
		self.parse_stdin_data()
		return self.env