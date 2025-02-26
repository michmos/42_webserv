#!/usr/bin/env python3

import sys
from ResponseGenerator import ResponseGenerator
from submit_contact_info.Parser import Parser
from submit_contact_info.CGI import CGI

def main() -> int:
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
		return 400

	if cgi_contact.get_status_code() == 200:
		return 0
	return cgi_contact.get_status_code()

if __name__ == '__main__':
	sys.exit(main())