import route_guide_pb2
import route_guide_pb2_grpc

class RouteGuideServicer(route_guide_pb2_grpc.RouteGuideServicer):

    def GetFeature(self, request, context):
        feature = get_feature(self.db, request)
        if feature is None:
            # create and return protobuf object
            return route_guide_pb2.Feature(name="", location=request)
        else:
            return feature

    def ListFeatures(self, request, context):
        left = min(request.lo.longitude, request.hi.longitude)
        right = max(request.lo.longitude, request.hi.longitude)
        top = max(request.lo.latitude, request.hi.latitude)
        bottom = min(request.lo.latitude, request.hi.latitude)
        
        # streaming with yield
        for feature in self.db:
            if (feature.location.longitude >= left and
                    feature.location.longitude <= right and
                    feature.location.latitude >= bottom and
                    feature.location.latitude <= top):
                yield feature