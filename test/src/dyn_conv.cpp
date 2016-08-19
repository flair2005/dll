//=======================================================================
// Copyright (c) 2014-2016 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <deque>

#include "dll_test.hpp"

#include "dll/dyn_conv_layer.hpp"
#include "dll/dyn_dense_layer.hpp"
#include "dll/dbn.hpp"
#include "dll/trainer/stochastic_gradient_descent.hpp"

#include "mnist/mnist_reader.hpp"
#include "mnist/mnist_utils.hpp"

TEST_CASE("dyn_conv/sgd/1", "[dense][dbn][mnist][sgd]") {
    using dbn_t = dll::dbn_desc<
        dll::dbn_layers<
            dll::dyn_conv_desc<dll::activation<dll::function::SIGMOID>>::layer_t,
            dll::dyn_dense_desc<dll::activation<dll::function::SIGMOID>>::layer_t>,
        dll::trainer<dll::sgd_trainer>, dll::batch_size<10>>::dbn_t;

    auto dataset = mnist::read_dataset_3d<std::vector, etl::dyn_matrix<float, 3>>(1000);
    REQUIRE(!dataset.training_images.empty());

    auto dbn = std::make_unique<dbn_t>();

    dbn->template layer_get<0>().init_layer(1, 28, 28, 10, 24, 24);
    dbn->template layer_get<1>().init_layer(10 * 24 * 24, 10);

    dbn->learning_rate = 0.05;

    auto ft_error = dbn->fine_tune(dataset.training_images, dataset.training_labels, 100);
    std::cout << "ft_error:" << ft_error << std::endl;

    CHECK(ft_error < 5e-2);

    auto test_error = dll::test_set(dbn, dataset.test_images, dataset.test_labels, dll::predictor());
    std::cout << "test_error:" << test_error << std::endl;
    REQUIRE(test_error < 0.2);
}
