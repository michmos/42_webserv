#!/usr/bin/env python3

from ResponseGenerator import ResponseGenerator
from submit_contact_info.Parser import Parser
from submit_contact_info.CGI import CGI
from generalChecks import general_checks

import sys
import os

def main() -> int:
	try:
		if general_checks(False):
			return 0
	except Exception as e:
		print(f"Error CGI: {e}", file=sys.stderr)
		return 1

	cgi_contact = CGI()

	try:
		cgi_contact.parsing()
		if cgi_contact.get_status_code() == 200:
			cgi_contact.process_request()
		cgi_contact.generate_response()
	except Exception as e:
		print(f"Error in CGI: {e}", file=sys.stderr)
		try:
			cgi_contact.set_status_code(400)
			cgi_contact.generate_response()
		except Exception as inner_e:
			print(f"Error with generate response: {e}", file=sys.stderr)
			return 1

	return 0

if __name__ == '__main__':
	sys.exit(main())