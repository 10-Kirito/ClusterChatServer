#ifndef SINGLETONTEMPLATE_H
#define SINGLETONTEMPLATE_H

template <typename T> class SingletonTemplate {
public:
  static T &getInstance() {
    static T instance;
    return instance;
  }
  virtual ~SingletonTemplate() = default;
  // delete copy constructor and assignment operator
  SingletonTemplate(const SingletonTemplate &) = delete;
  SingletonTemplate &operator=(const SingletonTemplate &) = delete;
protected:
  SingletonTemplate() = default;
};

#endif // SINGLETONTEMPLATE_H