
#include "settings.hpp"
#include "../tests.hpp"

#include <server/request_manager.hpp>

class NonPrerenderingRequestManager : public RequestManager 
{
public:
	NonPrerenderingRequestManager( 	const shared_ptr<Configuration>& config,
									const shared_ptr<Geodata>& data,
									const shared_ptr<Renderer>& renderer,
									const shared_ptr<Cache>& cache,
									const shared_ptr<StylesheetManager>& ssm
								 )
	: RequestManager(config, data, renderer, cache, ssm)
	, pseudoQueueSize(0)
	{
	}
	
	virtual ~NonPrerenderingRequestManager() {}
	//Disabled to prevent prerendering
	void enqueue(const shared_ptr<TileIdentifier>& ti)
	{
		pseudoQueueSize++;
	}
	int pseudoQueueSize;
};