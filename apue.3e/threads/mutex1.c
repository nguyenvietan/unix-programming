#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

struct foo {
	int             f_count;
	pthread_mutex_t f_lock;
	int             f_id;
	/* ... more stuff here ... */
};

struct foo *
foo_alloc(int id) /* allocate the object */
{
	struct foo *fp;

	if ((fp = malloc(sizeof(struct foo))) != NULL) {
		fp->f_count = 1;
		fp->f_id = id;
		if (pthread_mutex_init(&fp->f_lock, NULL) != 0) {
			free(fp);
			return(NULL);
		}
		/* ... continue initialization ... */
	}
	return(fp);
}

void
foo_hold(struct foo *fp) /* add a reference to the object */
{
	pthread_mutex_lock(&fp->f_lock);
	fp->f_count++;
	printf("Thread holding foo with ID %d, count: %d\n", fp->f_id, fp->f_count);
	pthread_mutex_unlock(&fp->f_lock);
}

void
foo_rele(struct foo *fp) /* release a reference to the object */
{
	pthread_mutex_lock(&fp->f_lock);
	if (--fp->f_count == 0) { /* last reference */
	    printf("Releasing foo with ID %d\n", fp->f_id);
		pthread_mutex_unlock(&fp->f_lock);
		pthread_mutex_destroy(&fp->f_lock);
		free(fp);
	} else {
        printf("Thread releasing reference to foo with ID %d, count: %d\n", fp->f_id, fp->f_count);
		pthread_mutex_unlock(&fp->f_lock);
	}
}

/* Thread function to manipulate foo */
void *thread_func(void *arg) {
    struct foo *myFoo = (struct foo *)arg;

    /* Hold the foo object (add a reference) */
    foo_hold(myFoo);

    /* Simulate some work */
    usleep(100000); /* Sleep for 100 milliseconds */

    /* Release the foo object (release a reference) */
    foo_rele(myFoo);

    return NULL;
}

int main() {

    /* Allocate a new foo object */
    struct foo *myFoo = foo_alloc(123);

    /* Create threads to manipulate the foo object */
    pthread_t tid[5];
    int i;
	for (i = 0; i < 5; i++) {
        pthread_create(&tid[i], NULL, thread_func, myFoo);
    }

    /* Join threads */
    for (i = 0; i < 5; i++) {
        pthread_join(tid[i], NULL);
    }

    return 0;
}
