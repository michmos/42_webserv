#!/usr/bin/env python3

from upload.Parser import Parser
from upload.UploadFile import UploadFile
from ResponseGenerator import ResponseGenerator
import sys

class CGI:
	def __init__( self ):
		self.env = {}
		self.status_code = 200
		self.body = ""
		
	def get_status_code( self ) -> int:
		return self.status_code

	def set_status_code( self, status_code:int ) -> None:
		self.status_code = status_code

	def parsing( self ) -> None:
		parser = Parser()
		self.env = parser.parsing_cgi_input()
		self.status_code = parser.get_status_code()
		if self.status_code != 200:
			return
		self.body = parser.get_body()

	def process_request( self ) -> None:
		uploader = UploadFile(self.env["filename"])
		self.status_code = uploader.upload_file(self.body)

	
	def write_to_stdout( self, response:str ) -> None:
		sys.stdout.write(response)
		sys.stdout.flush()

	def generate_response( self ) -> None:
		response = ResponseGenerator("POST", self.status_code)
		self.write_to_stdout(response.create_response())
