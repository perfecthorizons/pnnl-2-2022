import cherrypy
from os import path

class API:
    @cherrypy.expose
    def index(self):
        raise cherrypy.HTTPRedirect('http://' + cherrypy.request.remote.ip + ':80/static/index.html')
    
    @cherrypy.expose
    @cherrypy.tools.json_in()
    @cherrypy.tools.json_out()
    def start(self, *vpath, **parameters):
        return {"message": "started"}

    @cherrypy.expose
    @cherrypy.tools.json_in()
    @cherrypy.tools.json_out()
    def stop(self, *vpath, **parameters):
        pass

    @cherrypy.expose
    @cherrypy.tools.json_in()
    @cherrypy.tools.json_out()
    def shutdown(self, *vpath, **parameters):
        subprocess.Popen("sudo shutdown -t 10 -h")
        return {"message": "shutdown started", "time": 10}

    @cherrypy.expose
    @cherrypy.tools.json_in()
    @cherrypy.tools.json_out()
    def query(self, *vpath, **parameters):
        # Return run time information
        return {"message": "queried"}

    @cherrypy.expose
    @cherrypy.tools.json_in()
    @cherrypy.tools.json_out()
    def dyn_fetch(self, *vpath, **parameters):
        if len(vpath) == 0:
            return {
                "error": "No file specified."
            }
        elif len(vpath) >= 2:
            return {
                "error": "Ambiguous file specified."
            }

        try:
            with open(path.join("../main/dyn_functions/", vpath[0])) as f:
                return {
                    "code": "\n".join(f.readlines()).strip()
                }
        except IOError as e:
            return {
                "error": "The file '{}' does not exist or could not be opened.".format(vpath[0])
            }

    @cherrypy.expose
    @cherrypy.tools.json_in()
    @cherrypy.tools.json_out()
    def dyn_put(self, *vpath, **parameters):
        for parameter in parameters:
            print(parameter)

    @cherrypy.expose
    @cherrypy.tools.json_in()
    @cherrypy.tools.json_out()
    def dyn_compile(self, *vpath, **parameters):
        for parameter in parameters:
            print(parameter)

    @cherrypy.expose
    @cherrypy.tools.json_in()
    @cherrypy.tools.json_out()
    def dyn_inject(self, *vpath, **parameters):
        for parameter in parameters:
            print(parameter)


if __name__ == "__main__":
    api = API()

    config = {
        "global": {
            # "log.screen": False,
            "server.socket_host": "0.0.0.0",
            "server.socket_port": 8080,
        },
        "/": {}
    }
    cherrypy.quickstart(api, "/", config=config)
