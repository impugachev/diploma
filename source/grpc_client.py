import route_guide_pb2
import route_guide_pb2_grpc

# create channel
with grpc.insecure_channel('localhost:50051') as channel:

    # create stub
    stub = route_guide_pb2_grpc.RouteGuideStub(channel)

    # synchronous call
    feature = stub.GetFeature(point)

    # asynchronous call
    feature_future = stub.GetFeature.future(point)
    feature = feature_future.result()

    # response-streaming
    for feature in stub.ListFeatures(rectangle):
        print(feature)