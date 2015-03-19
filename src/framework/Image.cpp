#include "Image.hpp"
#include "framework/marshal/marshal.hpp"
#include "framework/NanCheck.hpp"
#include "framework/Job.hpp"
#include "framework/ImageSource.hpp"
#include "framework/Async.hpp"

#include <nan.h>

using namespace v8;

#define SETUP_FUNCTION(TYP) \
    NanEscapableScope();      \
    TYP *self = ObjectWrap::Unwrap<TYP>(args.This());

namespace cloudcv
{
    enum EncodeOutputFormat {
        EncodeOutputFormatJpeg,
        EncodeOutputFormatWebp,
        EncodeOutputFormatPng
    };

    class EncodeImageTask : public Job
    {
    public:
        EncodeImageTask(cv::Mat image, NanCallback * callback, EncodeOutputFormat fmt, bool returnDataUri = false)
            : Job(callback)
            , mImage(image)
            , mFormat(fmt)
            , mReturnDataUri(returnDataUri)
        {
        }

        virtual ~EncodeImageTask()
        {
        }

    protected:

        virtual void ExecuteNativeCode()
        {
            std::string mimeType;

            switch (mFormat)
            {
            case EncodeOutputFormatPng:
                cv::imencode(".png", mImage, mEncodedData);
                mimeType = "image/png";
                break;

            case EncodeOutputFormatJpeg:
                cv::imencode(".jpeg", mImage, mEncodedData);
                mimeType = "image/jpeg";
                break;

            case EncodeOutputFormatWebp:
                cv::imencode(".webp", mImage, mEncodedData);
                mimeType = "image/webp";
                break;

            default:
                SetErrorMessage("Given format is not supported");
                return;
            };

            if (mReturnDataUri)
            {
                mDataUriString << "data:" << mimeType << ";base64,";
                Base64Encode(mEncodedData, mDataUriString);
            }
        }

        // This function is executed in the main V8/JavaScript thread. That means it's
        // safe to use V8 functions again. Don't forget the HandleScope!
        virtual Local<Value> CreateCallbackResult()
        {
            NanEscapableScope();
            if (mReturnDataUri)
                return NanEscapeScope(marshal(mDataUriString.str()));
            else
                return NanEscapeScope(marshal(mEncodedData));
        }

    private:

        static void Base64Encode(const std::vector<uint8_t>& data, std::ostringstream& encoded_data)
        {
            static size_t mod_table[] = { 0, 2, 1 };
            static char encoding_table[] = {
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                '4', '5', '6', '7', '8', '9', '+', '/'
            };

            size_t input_length = data.size();
            //size_t output_length = 4 * ((input_length + 2) / 3);
            //encoded_data.resize(output_length);

            for (size_t i = 0; i < input_length;)
            {
                uint32_t octet_a = i < input_length ? data[i++] : 0;
                uint32_t octet_b = i < input_length ? data[i++] : 0;
                uint32_t octet_c = i < input_length ? data[i++] : 0;

                uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

                encoded_data << encoding_table[(triple >> 3 * 6) & 0x3F]
                    << encoding_table[(triple >> 2 * 6) & 0x3F]
                    << encoding_table[(triple >> 1 * 6) & 0x3F]
                    << encoding_table[(triple >> 0 * 6) & 0x3F];
            }

            // Extra padding
            for (size_t i = 0; i < mod_table[input_length % 3]; i++)
            {
                encoded_data << '=';
            }
        }

    private:
        cv::Mat                 mImage;
        EncodeOutputFormat      mFormat;
        bool                    mReturnDataUri;
        std::vector<uint8_t>    mEncodedData;
        std::ostringstream      mDataUriString;
    };

    v8::Persistent<v8::FunctionTemplate> ImageView::constructor;

    ImageView::ImageView(const cv::Mat& image)
        : mImage(image)
    {

    }

    NAN_METHOD(ImageView::Width)
    {
        SETUP_FUNCTION(ImageView);
        int width = self->mImage.cols;
        NanReturnValue(NanNew<v8::Integer>(width));
    }

    NAN_METHOD(ImageView::Height)
    {
        SETUP_FUNCTION(ImageView);
        int height = self->mImage.rows;
        NanReturnValue(NanNew<v8::Integer>(height));
    }

    NAN_METHOD(ImageView::Channels)
    {
        SETUP_FUNCTION(ImageView);
        NanReturnValue(NanNew<v8::Integer>(self->mImage.channels()));
    }

    NAN_METHOD(ImageView::Type)
    {
        SETUP_FUNCTION(ImageView);
        NanReturnValue(NanNew<v8::Integer>(self->mImage.type()));
    }

    NAN_METHOD(ImageView::Stride)
    {
        SETUP_FUNCTION(ImageView);
        // Temporary hack:
        // https://github.com/rvagg/nan/issues/270
        // https://github.com/BloodAxe/CloudCV/issues/3
        uint32_t stride = static_cast<uint32_t>(self->mImage.step[0]);
        NanReturnValue(NanNew<v8::Integer>(stride));
    }

    NAN_METHOD(ImageView::AsJpegStream)
    {
        SETUP_FUNCTION(ImageView);
        Local<Function> imageCallback;
        std::string error;
        
        if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(1)
            .Argument(0).IsFunction().Bind(imageCallback))
        {
            NanCallback *callback = new NanCallback(imageCallback);
            NanAsyncQueueWorker(new EncodeImageTask(self->mImage, callback, EncodeOutputFormatJpeg));
            NanReturnUndefined();
        }
        else if (!error.empty())
        {
            NanThrowTypeError(error.c_str());
        }
    }

    NAN_METHOD(ImageView::AsPngStream)
    {
        SETUP_FUNCTION(ImageView);
        Local<Function> imageCallback;
        std::string error;

        if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(1)
            .Argument(0).IsFunction().Bind(imageCallback))
        {
            NanCallback *callback = new NanCallback(imageCallback);
            NanAsyncQueueWorker(new EncodeImageTask(self->mImage, callback, EncodeOutputFormatPng));
            NanReturnUndefined();
        }
        else if (!error.empty())
        {
            NanThrowTypeError(error.c_str());
        }
    }

    NAN_METHOD(ImageView::Thumbnail)
    {
        SETUP_FUNCTION(ImageView);
        Local<Function> imageCallback;

        int w, h;
        std::string error;
        if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(3)
            .Argument(0).Bind(w)
            .Argument(1).Bind(h)
            .Argument(2).IsFunction().Bind(imageCallback))
        {
            NanCallback *callback = new NanCallback(imageCallback);

            auto task = [w,h, self](AsyncReturnHelper& result, AsyncErrorFunction error) {
                if (self->mImage.empty()) {
                     error("Image is empty");
                     return;                    
                }

                cv::Mat thumb;
                cv::resize(self->mImage, thumb, cv::Size(w,h));
                result(thumb);
            };
            
            Async(task, callback);
            NanReturnUndefined();
        }
        else if (!error.empty())
        {
            NanThrowTypeError(error.c_str());
        }
    }

    NAN_METHOD(ImageView::AsPngDataUri)
    {
        SETUP_FUNCTION(ImageView);
        Local<Function> imageCallback;
        std::string error;

        if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(1)
            .Argument(0).IsFunction().Bind(imageCallback))
        {
            NanCallback *callback = new NanCallback(imageCallback);
            NanAsyncQueueWorker(new EncodeImageTask(self->mImage, callback, EncodeOutputFormatPng, true));
            NanReturnValue(NanTrue());
        }
        else if (!error.empty())
        {
            NanThrowTypeError(error.c_str());
        }
    }

    template <typename T>
    struct PlainImageView
    {
        cv::Size size;
        int channels;
        std::string depth;
        std::vector<T> data;

        template <typename Archive>
        void serialize(Archive& ar)
        {
            ar & serialization::make_nvp("size", size);
            ar & serialization::make_nvp("channels", channels);
            ar & serialization::make_nvp("depth", channels);
            ar & serialization::make_nvp("data", data);
        }
    };

    NAN_METHOD(ImageView::AsObject)
    {
        SETUP_FUNCTION(ImageView);
        Local<Object> res = NanNew<Object>();
        cv::Mat image = self->mImage;


        size_t length = image.total() * image.channels();

        switch (image.depth())
        {
        case CV_8U:
        {
            PlainImageView<uint8_t> view;
            view.size = image.size();
            view.depth = "CV_8U";
            view.channels = image.channels();
            view.data.assign((uint8_t*)image.data, (uint8_t*)image.data + length);
            NanReturnValue(marshal(view));
        };            break;

        case CV_16U:
        {
            PlainImageView<uint16_t> view;
            view.size = image.size();
            view.depth = "CV_16U";
            view.channels = image.channels();
            view.data.assign((uint16_t*)image.data, (uint16_t*)image.data + length);
            NanReturnValue(marshal(view));
        };   break;

        case CV_16S:
        {
            PlainImageView<int16_t> view;
            view.size = image.size();
            view.depth = "CV_16S";
            view.channels = image.channels();
            view.data.assign((int16_t*)image.data, (int16_t*)image.data + length);
            NanReturnValue(marshal(view));
        };   break;

        case CV_32S:
        {
            PlainImageView<int32_t> view;
            view.size = image.size();
            view.depth = "CV_32S";
            view.channels = image.channels();
            view.data.assign((int32_t*)image.data, (int32_t*)image.data + length);
            NanReturnValue(marshal(view));
        };   break;

        case CV_32F:
        {
            PlainImageView<float> view;
            view.size = image.size();
            view.depth = "CV_32F";
            view.channels = image.channels();
            view.data.assign((float*)image.data, (float*)image.data + length);
            NanReturnValue(marshal(view));
        };   break;

        case CV_64F:
        {
            PlainImageView<double> view;
            view.size = image.size();
            view.depth = "CV_64F";
            view.channels = image.channels();
            view.data.assign((double*)image.data, (double*)image.data + length);
            NanReturnValue(marshal(view));
        };   break;

        default:
            break;
        };

        NanReturnUndefined();
    }


    NAN_METHOD(ImageView::AsJpegDataUri)
    {
        SETUP_FUNCTION(ImageView);
        Local<Function> imageCallback;
        std::string error;

        if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(1)
            .Argument(0).IsFunction().Bind(imageCallback))
        {
            NanCallback *callback = new NanCallback(imageCallback);
            NanAsyncQueueWorker(new EncodeImageTask(self->mImage, callback, EncodeOutputFormatJpeg, true));
            NanReturnValue(NanTrue());
        }
        else if (!error.empty())
        {
            NanThrowTypeError(error.c_str());
        }
    }

    void ImageView::Init(v8::Handle<v8::Object> exports)
    {
        //Class
        v8::Local<v8::FunctionTemplate> tpl = NanNew<FunctionTemplate>(ImageView::New);
        tpl->InstanceTemplate()->SetInternalFieldCount(1);
        tpl->SetClassName(NanNew("ImageView"));

        NODE_SET_PROTOTYPE_METHOD(tpl, "width", ImageView::Width);
        NODE_SET_PROTOTYPE_METHOD(tpl, "height", ImageView::Height);
        NODE_SET_PROTOTYPE_METHOD(tpl, "channels", ImageView::Channels);
        NODE_SET_PROTOTYPE_METHOD(tpl, "type", ImageView::Type);
        NODE_SET_PROTOTYPE_METHOD(tpl, "stride", ImageView::Stride);

        NODE_SET_PROTOTYPE_METHOD(tpl, "asJpegStream", ImageView::AsJpegStream);
        NODE_SET_PROTOTYPE_METHOD(tpl, "asJpegDataUri", ImageView::AsJpegDataUri);
        NODE_SET_PROTOTYPE_METHOD(tpl, "asPngStream", ImageView::AsPngStream);
        NODE_SET_PROTOTYPE_METHOD(tpl, "asPngDataUri", ImageView::AsPngDataUri);
        NODE_SET_PROTOTYPE_METHOD(tpl, "asObject", ImageView::AsObject);

        NODE_SET_PROTOTYPE_METHOD(tpl, "thumbnail", ImageView::Thumbnail);

        NanAssignPersistent(constructor, tpl);
        //constructor = Persistent<Function>::New();
        exports->Set(NanNew<String>("ImageView"), NanNew<FunctionTemplate>(constructor)->GetFunction());
        //std::cout << "ImageView::Init finished" << std::endl;
    }

    NAN_METHOD(loadImage)
    {
        TRACE_FUNCTION;
        NanEscapableScope();

        std::string     imagePath;
        std::string     error;
        Local<Object>   imageBuffer;
        Local<Function> loadCallback;

        if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(2)
            .Argument(0).IsString().Bind(imagePath)
            .Argument(1).IsFunction().Bind(loadCallback))
        {
            NanCallback *callback = new NanCallback(loadCallback);
            auto task = [imagePath](AsyncReturnHelper& result, AsyncErrorFunction error) {
                cv::Mat image = CreateImageSource(imagePath)->getImage();
                if (image.empty())
                    error("Cannot read image from the file");
                else
                    result(image);
            };
            
            Async(task, callback);
            NanReturnUndefined();
        }
        else if (NanCheck(args)
            .Error(&error)
            .ArgumentsCount(2)
            .Argument(0).IsBuffer().Bind(imageBuffer)
            .Argument(1).IsFunction().Bind(loadCallback))
        {
            NanCallback *callback = new NanCallback(loadCallback);
            auto task = [imagePath](AsyncReturnHelper& result, AsyncErrorFunction error) {
                cv::Mat image = CreateImageSource(imagePath)->getImage();
                if (image.empty())
                    error("Cannot decode image from the buffer");
                else
                    result(image);
            };
            
            Async(task, callback);
            NanReturnUndefined();
        }
        else if (!error.empty())
        {
            NanThrowTypeError(error.c_str());                
        }
    }

    NAN_METHOD(ImageView::New)
    {
        NanEscapableScope();

        if (args.This()->InternalFieldCount() == 0)
            return NanThrowError("Cannot instantiate without new");

        std::string filename = marshal<std::string>(args[0]);

        cv::Mat im = cv::imread(filename.c_str());
        auto imageView = new ImageView(im);

        imageView->Wrap(args.Holder());
        NanReturnValue(args.Holder());
    }

    v8::Local<v8::Value> ImageView::ViewForImage(cv::Mat image)
    {        
        NanEscapableScope();
        // Insiped by SO: http://stackoverflow.com/questions/16600735/what-is-an-internal-field-count-and-what-is-setinternalfieldcount-used-for
        Local<Object> holder = NanNew<FunctionTemplate>(constructor)->GetFunction()->NewInstance();

        ImageView * imageView = new ImageView(image);
        imageView->Wrap(holder);
        return NanEscapeScope(holder);
    }
}