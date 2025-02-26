#!/usr/bin/env python3

from ResponseGenerator import ResponseGenerator
from delete.DeleteFile import DeleteFile
from delete.Parser import Parser
import sys

class CGI:
	def __init__( self ):
		self.file_to_delete = ""
		self.status_code = 200
		self.body = ""
		
	def get_status_code( self ) -> int:
		return self.status_code

	def set_status_code( self, status_code:int ) -> None:
		self.status_code = status_code


	def parsing( self ) -> None:
		""" Checks method and extract filename """
		parser = Parser()
		self.file_to_delete = parser.parsing_cgi_input()
		self.status_code = parser.get_status_code()


	def process_request( self ) -> None:
		if self.file_to_delete == "":
			self.status_code = 400
			return
		deleter = DeleteFile(self.file_to_delete)
		self.status_code = deleter.delete_file()

		
	def write_to_stdout( self, response:str ) -> None:
		sys.stdout.write(response)
		sys.stdout.flush()


	def generate_response( self ) -> None:
		response = ResponseGenerator("DELETE", self.status_code)
		self.write_to_stdout(response.create_response())
