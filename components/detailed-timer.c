#include "urn-component.h"

typedef struct _UrnDetailedTimer {
    UrnComponent base;
    GtkWidget *detailed_timer;
    GtkWidget *detailed_info;
    GtkWidget *segment_pb;
    GtkWidget *segment_best;
    GtkWidget *detailed_time;
    GtkWidget *time;
    GtkWidget *time_seconds;
    GtkWidget *time_millis;
    GtkWidget *segment;
    GtkWidget *segment_seconds;
    GtkWidget *segment_millis;
} UrnDetailedTimer;
extern UrnComponentOps urn_detailed_timer_operations;

UrnComponent *urn_component_detailed_timer_new() {
    UrnDetailedTimer *self;
    GtkWidget *spacer;

    self = malloc(sizeof(UrnDetailedTimer));
    if (!self) return NULL;
    self->base.ops = &urn_detailed_timer_operations;

    self->detailed_timer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_show(self->detailed_timer);

    self->detailed_info = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(self->detailed_timer), self->detailed_info, FALSE, FALSE, 0);
    gtk_widget_show(self->detailed_info);

    self->segment_best = gtk_label_new(NULL);
    add_class(self->segment_best, "segment-best");
    gtk_box_pack_end(GTK_BOX(self->detailed_info), self->segment_best, FALSE, FALSE, 0);
    gtk_widget_show(self->segment_best);

    self->segment_pb = gtk_label_new(NULL);
    add_class(self->segment_pb, "segment-pb");
    gtk_box_pack_end(GTK_BOX(self->detailed_info), self->segment_pb, FALSE, FALSE, 0);
    gtk_widget_show(self->segment_pb);

    self->detailed_time = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    add_class(self->detailed_time, "timer");
    gtk_box_pack_end(GTK_BOX(self->detailed_timer), self->detailed_time, TRUE, TRUE, 0);
    gtk_widget_show(self->detailed_time);

    self->time = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    add_class(self->time, "timer");
    add_class(self->time, "time");
    gtk_box_pack_start(GTK_BOX(self->detailed_time), self->time, FALSE, FALSE, 0);
    gtk_widget_show(self->time);

    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_container_add(GTK_CONTAINER(self->time), spacer);
    gtk_widget_show(spacer);

    self->time_seconds = gtk_label_new(NULL);
    add_class(self->time_seconds, "timer-seconds");
    gtk_widget_set_valign(self->time_seconds, GTK_ALIGN_BASELINE);
    gtk_container_add(GTK_CONTAINER(self->time), self->time_seconds);
    gtk_widget_show(self->time_seconds);

    spacer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(spacer, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(self->time), spacer);
    gtk_widget_show(spacer);

    self->time_millis = gtk_label_new(NULL);
    add_class(self->time_millis, "timer-millis");
    gtk_widget_set_valign(self->time_millis, GTK_ALIGN_BASELINE);
    gtk_container_add(GTK_CONTAINER(spacer), self->time_millis);
    gtk_widget_show(self->time_millis);

    self->segment = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    add_class(self->segment, "segment");
    gtk_box_pack_start(GTK_BOX(self->detailed_time), self->segment, FALSE, FALSE, 0);
    gtk_widget_show(self->segment);

    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(spacer, TRUE);
    gtk_container_add(GTK_CONTAINER(self->segment), spacer);
    gtk_widget_show(spacer);

    self->segment_seconds = gtk_label_new(NULL);
    add_class(self->segment_seconds, "segment-seconds");
    gtk_widget_set_valign(self->segment_seconds, GTK_ALIGN_BASELINE);
    gtk_container_add(GTK_CONTAINER(self->segment), self->segment_seconds);
    gtk_widget_show(self->segment_seconds);

    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_valign(spacer, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(self->segment), spacer);
    gtk_widget_show(spacer);

    self->segment_millis = gtk_label_new(NULL);
    add_class(self->segment_millis, "segment-millis");
    gtk_widget_set_valign(self->segment_millis, GTK_ALIGN_BASELINE);
    gtk_container_add(GTK_CONTAINER(self->segment), self->segment_millis);
    gtk_widget_show(self->segment_millis);


    return (UrnComponent *)self;
}

// Avoid collision with timer_delete of time.h
static void urn_detailed_timer_delete(UrnComponent *self) {
    free(self);
}

static GtkWidget *detailed_timer_widget(UrnComponent *self) {
    return ((UrnDetailedTimer *)self)->detailed_timer;
}

static void detailed_timer_clear_game(UrnComponent *self_) {
    UrnDetailedTimer *self = (UrnDetailedTimer *)self_;
    gtk_label_set_text(GTK_LABEL(self->time_seconds), "");
    gtk_label_set_text(GTK_LABEL(self->time_millis), "");
    gtk_label_set_text(GTK_LABEL(self->segment_seconds), "");
    gtk_label_set_text(GTK_LABEL(self->segment_millis), "");

    remove_class(self->time, "behind");
    remove_class(self->time, "losing");

}

static void detailed_timer_draw(UrnComponent *self_, urn_game *game, urn_timer *timer) {
    UrnDetailedTimer *self = (UrnDetailedTimer *)self_;
    char str[256], millis[256], seg[256], seg_millis[256];
    char pb[256] = "PB:    ";
    char best[256] = "Best: ";
    int curr;

    curr = timer->curr_split;
    if (curr == game->split_count) {
        --curr;
    }

    remove_class(self->time, "delay");
    remove_class(self->time, "behind");
    remove_class(self->time, "losing");
    remove_class(self->time, "best-split");

    if (curr == game->split_count) {
        curr = game->split_count - 1;
    }
    if (timer->time <= 0) {
        add_class(self->time, "delay");
    } else {
        if (timer->curr_split == game->split_count
            && timer->split_info[curr]
               & URN_INFO_BEST_SPLIT) {
            add_class(self->time, "best-split");
        } else{
            if (timer->split_info[curr]
                & URN_INFO_BEHIND_TIME) {
                add_class(self->time, "behind");
            }
            if (timer->split_info[curr]
                & URN_INFO_LOSING_TIME) {
                add_class(self->time, "losing");
            }
        }
    }
    urn_time_millis_string(str, &millis[1], timer->time);
    millis[0] = '.';
    gtk_label_set_text(GTK_LABEL(self->time_seconds), str);
    gtk_label_set_text(GTK_LABEL(self->time_millis), millis);
    

    if (timer->curr_split == 0) {
        gtk_label_set_text(GTK_LABEL(self->segment_seconds), str);
        gtk_label_set_text(GTK_LABEL(self->segment_millis), millis);
    } else {
        urn_time_millis_string(seg, &seg_millis[1], timer->segment_times[timer->curr_split]);
        seg_millis[0] = '.';
        gtk_label_set_text(GTK_LABEL(self->segment_seconds), seg);
        gtk_label_set_text(GTK_LABEL(self->segment_millis), seg_millis);
    }

    urn_time_string(&pb[6], game->segment_times[timer->curr_split]);
    gtk_label_set_text(GTK_LABEL(self->segment_pb), pb);

    urn_time_string(&best[6], game->best_segments[timer->curr_split]);
    gtk_label_set_text(GTK_LABEL(self->segment_best), best);

}

UrnComponentOps urn_detailed_timer_operations = {
    .delete = urn_detailed_timer_delete,
    .widget = detailed_timer_widget,
    .clear_game = detailed_timer_clear_game,
    .draw = detailed_timer_draw
};
