#!/usr/bin/env python3

from upload_delete.file_operations import DeleteFile, UploadFile
from upload_delete.response_generator import ResponseGenerator
from upload_delete.parser import Parser
import sys

class CGI:
	def __init__( self ):
		self.env = {}
		self.status_code = 200
		self.body = ""
		
	def get_status_code( self ):
		return self.status_code

	def set_status_code( self, status_code):
		self.status_code = status_code
	
	def parsing( self ):
		parser = Parser()
		self.env = parser.parsing_cgi_input()
		self.status_code = parser.get_status_code()
		if self.status_code != 200:
			return
		self.body = parser.get_body()
	
	def process_request( self ):
		if self.status_code != 200:
			return
		if self.env["method"] == "POST":
			uploader = UploadFile(self.env["filename"])
			self.status_code = uploader.upload_file(self.body)
		elif self.env["method"] == "DELETE":
			deleter = DeleteFile(self.env["request_target"])
			self.status_code = deleter.delete_file()
		else:
			self.status_code = 405
		
	def write_to_stdout( self, response ):
		sys.stdout.write(response)
		sys.stdout.flush()

	def generate_response( self ):
		response = ResponseGenerator(self.env["method"], self.status_code)
		self.write_to_stdout(response.create_response())
