/*
 * shared objects for native management
 */
var __shared_object_holder__    = __shared_object_holder__ || {};
var __shared_object_id__        = __shared_object_id__ || 0;

function __register_shared_object(obj)
{
        if(obj.__soi__ == undefined) {
                __shared_object_id__                    += 1;
                obj.__soi__                             = __shared_object_id__;
                __shared_object_holder__[obj.__soi__]   = obj;
        }
}

function __remove_shared_object(obj)
{
        if(obj.__soi__ != undefined) {
                __shared_object_holder__[obj.__soi__]   = undefined;
                obj.__soi__                             = undefined;
        }
}

function __shared_object_get(id)
{
        return __shared_object_holder__[id];
}

function __shared_object_free(id)
{
        var obj = __shared_object_holder__[id];
        if(obj != null && obj != undefined && obj.free != null && obj.free != undefined) {
                obj.free();
        }
}
