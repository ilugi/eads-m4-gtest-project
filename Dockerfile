FROM gcc:latest
RUN apt-get update
RUN apt-get install -y google-mock cmake
RUN cd /usr/src/gtest && cmake . && make
RUN cp /usr/src/gtest/libgtest* /usr/lib/
RUN cd /usr/src/gmock && cmake . && make
RUN cp /usr/src/gmock/libgmock* /usr/lib/



