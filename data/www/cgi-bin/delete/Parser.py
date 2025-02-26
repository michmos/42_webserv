#!/usr/bin/env python3

import os
import urllib.parse
import cgi
import sys
import re

class Parser:
	def __init__( self ):
		self.status_code = 200


	def get_status_code( self ) -> int:
		return self.status_code


	def parsing_cgi_input( self ) -> str:
		"""
			Check method == DELETE request
			Returns filename extract from request target
		"""
		if "REQUEST_METHOD" in os.environ:
			if os.getenv('REQUEST_METHOD') != "DELETE":
				print(os.getenv('REQUEST_METHOD'), file=sys.stderr)
				print("Error: wrong request method", file=sys.stderr)
				self.status_code = 400
				return
		else:
			print("Error: no request method", file=sys.stderr)
			self.status_code = 400
			return

		if "REQUEST_TARGET" in os.environ:
			file = os.getenv('REQUEST_TARGET')
			if "../" in file:
				self.status_code = 403
		else:
			file = ""
		return file