#include <api.h>

void Queries_query_changed_after_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_new(world, Position);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_delete() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_delete(world, e1);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_add() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, 0);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_add(world, e1, Position);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_remove() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_remove(world, e1, Position);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_changed_after_set() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    
    ecs_entity_t e1 = ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_set(world, e1, Position, {10, 20});
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Sys(ecs_iter_t *it) { }

void Queries_query_change_after_out_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Sys, EcsOnUpdate, [out] Position);
    
    ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_progress(world, 0);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_query_change_after_in_system() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);

    ECS_SYSTEM(world, Sys, EcsOnUpdate, [in] Position);
    
    ecs_new(world, Position);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);
    test_assert(ecs_query_changed(q) == true);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_progress(world, 0);
    test_assert(ecs_query_changed(q) == false);

    ecs_query_iter(q);
    test_assert(ecs_query_changed(q) == false);

    ecs_fini(world);
}

void Queries_subquery_match_existing() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);
    ECS_TYPE(world, Type, Position, Velocity);

    ecs_bulk_new(world, Position, 3);
    ecs_bulk_new(world, Velocity, 3);
    ecs_bulk_new(world, Type, 3);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);

    ecs_query_t *sq = ecs_subquery_new(world, q, "Velocity");
    test_assert(sq != NULL);

    int32_t table_count = 0, entity_count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
        }
    }

    test_int(table_count, 2);
    test_int(entity_count, 6);

    table_count = 0, entity_count = 0;
    it = ecs_query_iter(sq);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
            test_assert(ecs_has(world, it.entities[i], Velocity));
        }
    } 

    test_int(table_count, 1);
    test_int(entity_count, 3);  

    ecs_fini(world);
}

void Queries_subquery_match_new() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);

    ecs_query_t *sq = ecs_subquery_new(world, q, "Velocity");
    test_assert(sq != NULL);

    ECS_TYPE(world, Type, Position, Velocity);
    ecs_bulk_new(world, Position, 3);
    ecs_bulk_new(world, Velocity, 3);
    ecs_bulk_new(world, Type, 3);

    int32_t table_count = 0, entity_count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
        }
    }

    test_int(table_count, 2);
    test_int(entity_count, 6);

    table_count = 0, entity_count = 0;
    it = ecs_query_iter(sq);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
            test_assert(ecs_has(world, it.entities[i], Velocity));
        }
    } 

    test_int(table_count, 1);
    test_int(entity_count, 3);  

    ecs_fini(world);
}

void Queries_subquery_inactive() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ECS_TYPE(world, Type, Position, Velocity);
    ecs_bulk_new(world, Position, 3);
    ecs_bulk_new(world, Velocity, 3);
    ecs_entity_t e = ecs_new(world, Type);

    ecs_query_t *q = ecs_query_new(world, "Position");
    test_assert(q != NULL);

    ecs_query_t *sq = ecs_subquery_new(world, q, "Velocity");
    test_assert(sq != NULL);
    
    /* Create an empty table which should deactivate it for both queries */
    ecs_delete(world, e);

    int32_t table_count = 0, entity_count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 3);

    table_count = 0, entity_count = 0;
    it = ecs_query_iter(sq);
    test_int(it.table_count, 0);
    test_int(it.inactive_table_count, 1);

    table_count = 0, entity_count = 0;
    it = ecs_query_iter(sq);
    while (ecs_query_next(&it)) {
        table_count ++;
    }

    test_int(table_count, 0);

    ecs_fini(world);
}

void Queries_subquery_rematch() {
    ecs_world_t *world = ecs_init();

    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, Velocity);

    ecs_entity_t parent = ecs_new(world, 0);
    ecs_add(world, parent, Position);

    ecs_entity_t e1 = ecs_new(world, 0);
    ecs_add(world, e1, Position);
    ecs_add(world, e1, Velocity);
    ecs_add_entity(world, e1, ECS_CHILDOF | parent);

    ecs_entity_t e2 = ecs_new(world, 0);
    ecs_add(world, e2, Position);
    ecs_add_entity(world, e2, ECS_CHILDOF | parent);

    ecs_query_t *q = ecs_query_new(world, "Position, PARENT:Position");
    test_assert(q != NULL);

    ecs_query_t *sq = ecs_subquery_new(world, q, "Velocity");
    test_assert(sq != NULL);

    int32_t table_count = 0, entity_count = 0;
    ecs_iter_t it = ecs_query_iter(q);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
        }
    }

    test_int(table_count, 2);  
    test_int(entity_count, 2);

    table_count = 0, entity_count = 0;
    it = ecs_query_iter(sq);
    while (ecs_query_next(&it)) {
        table_count ++;
        entity_count += it.count;

        int32_t i;
        for (i = 0; i < it.count; i ++) {
            test_assert(ecs_has(world, it.entities[i], Position));
            test_assert(ecs_has(world, it.entities[i], Velocity));
        }
    }

    test_int(table_count, 1);
    test_int(entity_count, 1);

    /* Queries now no longer match */
    ecs_remove(world, parent, Position);

    /* Force rematching */
    ecs_progress(world, 0);

    /* Test if tables have been unmatched */
    it = ecs_query_iter(q);
    test_int(it.table_count, 0);

    it = ecs_query_iter(sq);
    test_int(it.table_count, 0);
}
