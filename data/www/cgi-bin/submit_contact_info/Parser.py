#!/usr/bin/env python3

import os
import urllib.parse
import cgi
import sys
import re

class Parser:
	def __init__( self ):
		self.content_length = 0
		self.status_code = 200
		self.fields = {}
		self.body = ""
	
	def get_fields( self ) -> dict[str,str]:
		return self.fields

	def is_in_env( self, name:str ) -> bool:
		if name not in os.environ:
			return False
		return True

	# Get the needed env from the environ and saves it in a dictonary
	def extract_env( self ) -> int:
		if self.is_in_env("REQUEST_METHOD") and os.getenv('REQUEST_METHOD') == "POST":
			if self.is_in_env("CONTENT_LENGTH"):
				self.content_length = int(os.getenv('CONTENT_LENGTH'))
				if self.content_length <= 0:
					print(f"Error, no (valid) content length", file=sys.stderr)
					return 400
			else:
				print(f"Error, no (valid) content length", file=sys.stderr)
				return 400

			if "CONTENT-TYPE" in os.environ:
				if "application/x-www-form-urlencoded" != os.getenv('CONTENT-TYPE'):
					print(f"Error, no valid content-type", file=sys.stderr)
					return 400
			return 200
		else:
			print(f"Error, no request method", file=sys.stderr)
			return 400
	
	# With POST request, reads the input from STDIN, extract header and body
	def parse_stdin_data( self ) -> None:
		self.body = sys.stdin.buffer.read(self.content_length)

	def get_field_information( self ) -> None:
		if isinstance(self.body, bytes):
			body = self.body.decode('utf-8')
		else:
			body = self.body
		fields = body.split('&')
		for field in fields:
			if "=" in field:
				index = field.find('=')
				key = field[:index]
				value = field[index + 1:]
				self.fields[key] = value

	def parsing_cgi_input( self ) -> int:
		self.status_code = self.extract_env()
	
		if self.status_code == 200:
			self.parse_stdin_data()

		if self.status_code == 200:
			self.get_field_information()

		return self.status_code