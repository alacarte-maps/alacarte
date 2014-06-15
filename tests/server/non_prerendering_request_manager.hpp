
#include "settings.hpp"
#include "../tests.hpp"

#include <server/request_manager.hpp>

class NonPrerenderingRequestManager : public RequestManager 
{
public:
	NonPrerenderingRequestManager( 	const std::shared_ptr<Configuration>& config,
									const std::shared_ptr<Geodata>& data,
									const std::shared_ptr<Renderer>& renderer,
									const std::shared_ptr<Cache>& cache,
									const std::shared_ptr<StylesheetManager>& ssm
								 )
	: RequestManager(config, data, renderer, cache, ssm)
	, pseudoQueueSize(0)
	{
	}
	
	virtual ~NonPrerenderingRequestManager() {}
	//Disabled to prevent prerendering
	void enqueue(const std::shared_ptr<TileIdentifier>& ti)
	{
		pseudoQueueSize++;
	}
	int pseudoQueueSize;
};