
import os
import urllib.parse
import cgi
import sys
import re

class Parser:
	def __init__( self ):
		self.env = {}
		self.status_code = 200
		self.body = ""
	
	def get_status_code( self ) -> int:
		return self.status_code
	
	def get_body( self ) -> str:
		return self.body

	def is_in_env( self, name ) -> bool:
		if name not in os.environ:
			return False
		return True

	# Get the needed env from the environ and saves it in a dictonary
	def extract_env( self ) -> dict:
		if self.is_in_env("CONTENT_LENGTH"):
			self.env["content_length"] = int(os.getenv('CONTENT_LENGTH'))
		if self.is_in_env("REQUEST_TARGET"):
			self.env["request_target"] = os.getenv('REQUEST_TARGET')
		else:
			print("extract env first", file=sys.stderr)
			self.status_code = 400
			return
		if "CONTENT-TYPE" in os.environ:
			print("ct:" , os.getenv('CONTENT-TYPE'), file=sys.stderr)
			self.env["content_type"] = os.getenv('CONTENT-TYPE')
		elif self.env["method"] == "POST":
			self.status_code = 400
			print("extract env post", file=sys.stderr)
			return
		else:
			content_type = ""
	
	# True if filename has right extension (Have to check it with conf?)
	def is_valid_extension( self ) -> bool:
		allowed_extensions = {"jpg", "jpeg", "png", "txt"}
		if self.env["filename"]:
			splitter = self.env["filename"].split(".")
			if splitter[-1].lower() not in allowed_extensions:
				self.status_code = 400
				return False
			else:
				return True
		return False

	# Extract the information if a Query_string is given
	def parse_query_string( self ):
		if "QUERY_STRING" not in os.environ():
			return
		query_string = os.getenv('QUERY_STRING', None)
		if query_string == None:
			return
		self.env.update(dict(urllib.parse.parse_qsl(query_string)))

	# Extract the filename from the header and saves it as a key-value pair in the env
	def save_filename_in_env( self, header ):
		first_index = header.find(b"filename=") + len(b"filename=") + 1
		second_index = header.find(b"\"", first_index)
		if (first_index == -1 or second_index == -1):
			self.status_code = 400
			print("save filename faalt", file=sys.stderr)
		else:
			print(header[first_index:second_index].decode(), file=sys.stderr)
		self.env["filename"] = header[first_index:second_index].decode()

	# Get the boundary from Content_type
	def get_boundary( self, content_type ):
		start = content_type.find("boundary=") + 9
		if start == -1:
			print("boundary parse stdin data", file=sys.stderr)
			self.status_code = 400
			return
		boundary = b"--" + content_type[start:].encode()
		return boundary

	# Extract the information between the boundary value
	def extract_boundary_data( self, boundary, post_data ):
		first_index = post_data.find(boundary) + len(boundary)
		second_index = post_data.find(boundary, first_index + 5)
		if first_index == -1:
			self.status_code = 400
			print("boundary parse stdin data", file=sys.stderr)
			return
		if (second_index == -1):
			boundary_data = post_data[first_index:].strip()
		else:
			boundary_data = post_data[first_index:second_index].strip()
		return boundary_data

	# Returns True when Content Length is not set or <= 0
	def	no_content_length( self ) -> bool:
		if not self.env["content_length"] or self.env["content_length"] <= 0:
			print("content length parse stdin data", file=sys.stderr)
			self.status_code = 400
			return True
		return False

	# With POST request, reads the input from STDIN, extract header and body and checks filename
	def parse_stdin_data( self ):
		if self.no_content_length():
			return
		post_data = sys.stdin.buffer.read(self.env["content_length"])

		boundary = self.get_boundary(self.env["content_type"])
		boundary_data = self.extract_boundary_data(boundary, post_data)
		if b"\r\n\r\n" not in boundary_data:
			self.status_code = 400
			print("boundary_data has no header/body", file=sys.stderr)
			return
		header, self.body = boundary_data.split(b"\r\n\r\n", 1)

		self.save_filename_in_env(header)
		print(self.env["filename"], file=sys.stderr)
		if not self.is_valid_extension():
			print("No valid Extension file", file=sys.stderr)
			self.status_code = 400

	def	get_file_for_delete( self ):
		if self.is_in_env("REQUEST_TARGET"):
			return os.getenv('REQUEST_TARGET')
		return ""

	# check method and handles or POST or DELETE request
	# returns env from Server and Stdin
	def parsing_cgi_input( self ):
		if self.is_in_env("REQUEST_METHOD"):
			self.env["method"] = os.getenv('REQUEST_METHOD')
		else:
			self.status_code = 400
			return self.env

		# POST or DELETE REQUEST
		if self.env["method"] == "POST":
			self.extract_env()
			if self.status_code != 200:
				return self.env
			self.parse_query_string()
			self.parse_stdin_data()
		elif self.env["method"] == "DELETE":
			self.env["request_target"] = self.get_file_for_delete()
		else:
			self.status_code = 400
		return self.env