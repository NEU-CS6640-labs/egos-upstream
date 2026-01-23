// ------- test cases for Lab2 ----------

/** Multi-threading functions from ult.c**/
void thread_init();
void thread_create();
void thread_yield();
void thread_exit();

int my_printf(const char* format, ...);

/* Testing thread function */
#define NULL 0
#define STACK_SIZE 4096

static void hello(void *arg){
    if (arg == NULL) {
        my_printf("Hello without arg\n");
    } else {
        my_printf("Hello with %s\n", arg);
    }
}

static void stack(void *arg){
    static int i = 128;
    while(i>0) {
        --i;
        my_printf("stak: nested call %d\n", i);
        stack(NULL);
    }

    if (arg != NULL) {
        my_printf("stack is %s\n", arg);
    }
}

static void many_yield(void *arg) {
    int i = 0;
    for (; i<102400; i++) {
        thread_yield();
    }
    my_printf("pass: many yield[%s]: i=%d\n", arg, i);
}

/** Producer and consumer functions **/

#define NSLOTS 3

static char *slots[NSLOTS];
static unsigned int in, out;

static void producer(void *arg){
    for (;;) {
        // first make sure there's an empty slot.
        // then add an entry to the slots
        // lastly, let other threads run

        while(slots[in] != (void*)0) {
            thread_yield();
        }

        slots[in++] = arg;
        if (in == NSLOTS) in = 0;
        thread_yield();
    }
}

static void consumer(void *arg){
    for (int i = 0; i < 5; i++) {
        // first make sure there's something in the buffer
        // then grab an entry to the slot
        // lastly, let other threads run

        while(slots[out] == (void*)0) {
            thread_yield();
        }

        void *x = slots[out];
        slots[out] = (void*)0;
        if (++out == NSLOTS) out = 0;
        my_printf("%s: got '%s'\n", arg, x);
        thread_yield();
    }
}

/* if you pass this, you should see
 *  "Hello with world"
 */
void test_create() {
    thread_init();
    thread_create(hello, "world", STACK_SIZE);
    thread_exit();
}

/* if you pass this, you should see
 * """
 * stak: nested call 127
 * ...
 * stak: nested call 0
 * stack is fine
 * """
 */
void test_stack() {
    thread_init();
    thread_create(stack, "fine", 4*STACK_SIZE);
    thread_exit();
}

/* if you pass this, you should see:
 * """
 * Hello without arg
 * Hello with world 1
 * Hello with world 2
 * Hello with main
 * """
 */
void test_exit() {
    thread_init();
    thread_create(hello, NULL, STACK_SIZE);
    thread_create(hello, "world 1", STACK_SIZE);
    thread_create(hello, "world 2", STACK_SIZE);
    hello("main");
    thread_exit();
}

/* if you pass this, you should see:
 *  "pass: many yield[single]: i=102400"
 */
void test_single_yield() {
    thread_init();
    thread_create(many_yield, "single", STACK_SIZE);
    thread_exit();
}

/* if you pass this, you should see:
 * """
 * pass: many yield[ping]: i=102400
 * pass: many yield[pong]: i=102400
 * """
 */
void test_pingpong_yield() {
    thread_init();
    thread_create(many_yield, "ping", STACK_SIZE);
    thread_create(many_yield, "pong", STACK_SIZE);
    thread_exit();
}

/* if you pass this, you should see:
 * """
 * consumer 5: got 'producer 1'
 * consumer 5: got 'producer 2'
 * consumer 4: got 'producer 1'
 * consumer 3: got 'producer 1'
 * consumer 2: got 'producer 2'
 * consumer 1: got 'producer 1'
 * consumer 5: got 'producer 1'
 * consumer 4: got 'producer 2'
 * consumer 3: got 'producer 1'
 * consumer 2: got 'producer 1'
 * consumer 1: got 'producer 2'
 * consumer 5: got 'producer 1'
 * consumer 4: got 'producer 1'
 * consumer 3: got 'producer 2'
 * consumer 2: got 'producer 1'
 * consumer 1: got 'producer 1'
 * consumer 5: got 'producer 2'
 * consumer 4: got 'producer 1'
 * consumer 3: got 'producer 1'
 * consumer 2: got 'producer 2'
 * consumer 1: got 'producer 1'
 * consumer 4: got 'producer 1'
 * consumer 3: got 'producer 2'
 * consumer 2: got 'producer 1'
 * consumer 1: got 'producer 1'
 * <then, running forever>
 * """
 */
void test_producer_consumer() {
    thread_init();

    thread_create(consumer, "consumer 1", STACK_SIZE);
    thread_create(consumer, "consumer 2", STACK_SIZE);
    thread_create(consumer, "consumer 3", STACK_SIZE);
    thread_create(consumer, "consumer 4", STACK_SIZE);
    thread_create(consumer, "consumer 5", STACK_SIZE);
    thread_create(producer, "producer 1", STACK_SIZE);
    producer("producer 2");
    thread_exit();
}
