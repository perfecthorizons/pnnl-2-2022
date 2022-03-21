import cherrypy
from os import path


class Home:
	@cherrypy.expose
	def index(self):
		raise cherrypy.HTTPRedirect('/static/index.html')

if __name__ == "__main__":

	home = Home()

	config = {
		"global": {
			#"log.screen": False,
			"server.socket_host": "0.0.0.0",
			"server.socket_port": 80,
		},
		"/": {
			"tools.staticdir.root": path.dirname(path.abspath(__file__))
		},
		"/static": {
			"tools.staticdir.on": True,
			"tools.staticdir.dir": "static"
		}
	}
	cherrypy.quickstart(home, "/", config=config)
