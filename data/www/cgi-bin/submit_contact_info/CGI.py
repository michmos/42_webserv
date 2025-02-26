#!/usr/bin/env python3

from submit_contact_info.Parser import Parser
from submit_contact_info.SaveForm import SaveForm
from ResponseGenerator import ResponseGenerator
import sys

class CGI:
	def __init__( self ):
		self.status_code = 200
		self.fields = {}
		
	def get_status_code( self ) -> int:
		return self.status_code

	def set_status_code( self, status_code: int ) -> None:
		self.status_code = status_code
	
	def parsing( self ) -> None:
		parser = Parser()
		self.status_code = parser.parsing_cgi_input()
		self.fields = parser.get_fields()
	
	def process_request( self ) -> None:
		form = SaveForm(self.fields)
		form.store_form_information()
		
	def write_to_stdout( self, response: str ) -> None:
		sys.stdout.write(response)
		sys.stdout.flush()

	def generate_response( self ) -> None:
		response = ResponseGenerator("POST", self.status_code)
		self.write_to_stdout(response.create_response())
