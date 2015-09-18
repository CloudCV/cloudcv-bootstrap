


void HandleArgument(
    std::string algorithm, 
    Nan::Arguments input, 
    Nan::Function callback)
{
    auto context = Algorithm::create(algorithm);
    context.runAsync(input, callback)
}

Nan::Arguments HandleArgument(
    std::string algorithm, 
    Nan::Arguments input)
{
    auto context = Algorithm::create(algorithm);
    return context.run(input)
}

// -----------------

AlgorithmContext Algorithm::register(string name, std::function<AlgorithmContext()> constructor)
{
    static std::map<std::string, std::function<AlgorithmContext()> > s_algorithms;

    return s_algorithms[name]();
}

// -----------------

template <class T, class... Ts>
struct elem_type_holder<0, std::tuple<T, Ts...>> {
  typedef T type;
};

template <size_t k, class T, class... Ts>
struct elem_type_holder<k, std::tuple<T, Ts...>> {
  typedef typename elem_type_holder<k - 1, std::tuple<Ts...>>::type type;
};

struct image
{
    typedef cv::Mat value_type;
    static const char * name = "image";
};

struct rho
{
    typedef float value_type;
    static const char * name = "rho";
};

struct theta
{
    typedef float value_type;
    static const char * name = "theta";
};

struct threshold
{
    typedef float value_type;
    static const char * name = "threshold";
};

template <typename Args..>
struct input_args
{
    static const size = sizeof...(Args);
};

template <typename Args..>
struct output_args
{
    static const size = sizeof...(Args);
};

template <typename in_args, typename out_args>
class Algorithm
{
public:
    typedef in_args  input_arguments;
    typedef out_args output_arguments;

    Algorithm(const char * name)
    {

    }
};

class HoughLines : Algorithm
<
input_args<image, rho, theta, threshold>,
output_args<lines>
>
{
public:


};