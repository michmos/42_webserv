#!/usr/bin/env python3

import sys
import os

class DeleteFile:
	def __init__( self, filename:str ):
		if filename[0] == '/':
			self.file = "data/uploads" + filename
		else:
			self.file = "data/uploads/" + filename
	
	def is_existing_file( self ) -> bool:
		if os.path.isfile(self.file):
			return True 
		else:
			return False
	
	def delete_file( self ) -> int:
		if self.is_existing_file():
			os.remove(self.file)
			return 200
		print(f"file {self.file} does not exist", file=sys.stderr)
		return 404
