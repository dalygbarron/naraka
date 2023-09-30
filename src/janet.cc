    // Initialize the virtual machine. Do this before any calls to Janet functions.
    janet_init();

    // Get the core janet environment. This contains all of the C functions in the core
    // as well as the code in src/boot/boot.janet.
    JanetTable *env = janet_core_env(NULL);

    std::ifstream ifs("data/test.janet");
  std::string content( (std::istreambuf_iterator<char>(ifs) ),
                       (std::istreambuf_iterator<char>()    ) );
    printf("%s\n", content.c_str());

    // One of several ways to begin the Janet vm.
    Janet response;
    janet_dostring(env, content.c_str(), "main", &response);
    if (janet_checktype(response, JANET_FUNCTION)) {
        JanetFunction *func = janet_unwrap_function(response);
        JanetFiber *fiber = janet_fiber(func, 5, 0, NULL);
        JanetSignal signal = JANET_SIGNAL_YIELD;
        while (signal == JANET_SIGNAL_YIELD) {
            signal = janet_continue(fiber, janet_wrap_number(0.34), &response);
        }
        printf("s %d\n", signal);
    } else {
        printf("fucked %d\n", janet_type(response));
    }
    // Use this to free all resources allocated by Janet.
    janet_deinit();