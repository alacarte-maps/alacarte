Änderungen zum Entwurf	{#changes}
======================

Tile
------
~~~~~~~~~~~~~~
void setImage(char *image); -> void setImage(vector<unsigned char>* image);
getLength() entfernt.
~~~~~~~~~~~~~~

Cache
------
~~~~~~~~~~~~~~
void Cache::deleteTiles(const Stylesheet& sheet) -> void Cache::deleteTiles(const string path)
~~~~~~~~~~~~~~

HttpRequest
-----------
~~~~~~~~~~~~~~
shared_ptr<HttpRequest> HttpRequest::Create( const string& httpMessage, const socket_ptr& socket ) entfernt
HttpRequest ( boost::asio::io_service &ioService, HttpServer *server, const shared_ptr<RequestManager> &manager ) hinzugefügt
~~~~~~~~~~~~~~

Jobs
--------
Job Eltern Klasse eingefügt


Configuration
-------------

~~~~~~~~~~~~~

Configuration(boost::option_description* desc, int argc, char** argv)

->

Configuration::Configuration(	boost::program_options::options_description& cmd_desc,
								boost::program_options::options_description& config_desc,
								boost::program_options::positional_options_description& pos_desc,
								int argc, char** argv)

~~~~~~~~~~~~~



Stylesheet
----------

~~~~~~~~~~

	TESTABLE void match(const shared_ptr<std::vector<int> >& nodeIDs,
						const shared_ptr<std::vector<int> >& wayIDs,
						const shared_ptr<std::vector<int> >& relIDs,
						RenderAttributes* styleMap) const;

->

	TESTABLE void match(const shared_ptr<std::vector<int> >& nodeIDs,
						const shared_ptr<std::vector<int> >& wayIDs,
						const shared_ptr<std::vector<int> >& relIDs,
						const shared_ptr<TileIdentifier>& ti,
						RenderAttributes* styleMap) const;

(da fürs Matchen auch die Zoomstufe gebraucht wird, z.B. node|z1-11 oder so.)

~~~~~~~~~

