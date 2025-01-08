#pragma once
#include "stdafx.h"
#include <memory>
#include "IDrawCommon.h"
#include "DrawCommon.h"
#include "TaskBarDlgDrawCommon.h"

template <class T>
struct StackObjectDeleter
{
    using pointer = std::add_pointer_t<T>;
    void operator()(pointer p_stack_object) const noexcept
    {
        if (p_stack_object)
        {
            Destroy(p_stack_object);
        }
    }
};
using UniqueIDrawCommon = std::unique_ptr<void, StackObjectDeleter<IDrawCommon>>;
using UniqueIDrawBuffer = std::unique_ptr<void, StackObjectDeleter<IDrawBuffer>>;

/**
 * @brief 新增渲染器类型后，将新类型写到下面类型的模板参数中
 *
 */
using DrawCommonUnionStorage = AlignedUnionStorage<CDrawCommon, CTaskBarDlgDrawCommon>;
/**
 * @brief 新增缓冲器（即析构时提交窗口绘制内容到系统的对象）类型后，将新类型写入到下面类型的模板参数中
 *
 */
using DrawBufferUnionStorage = AlignedUnionStorage<CDrawDoubleBuffer, CTaskBarDlgDrawBuffer, CTaskBarDlgDrawBufferUseDComposition>;
struct InvolvedDrawCommonStorages
{
    DrawBufferUnionStorage m_draw_buffer_union_storage{};
    DrawCommonUnionStorage m_draw_common_union_storage{};
};
/**
 * @brief 具有DrawCommon和DrawBuffer栈内存以及它们对应独占指针的栈内存块，
 注意：它会先析构DrawCommon再析构DrawBuffer
 *
 */
struct AllInvolvedDrawCommonObjectsStorage
{
    InvolvedDrawCommonStorages m_storage{};
    // 先析构DrawCommon再析构DrawBuffer
    UniqueIDrawBuffer m_unique_draw_buffer{};
    UniqueIDrawCommon m_unique_draw_common{};

    AllInvolvedDrawCommonObjectsStorage() = default;
    ~AllInvolvedDrawCommonObjectsStorage() = default;
    // 禁用复制移动
    AllInvolvedDrawCommonObjectsStorage(const AllInvolvedDrawCommonObjectsStorage&) = delete;
    AllInvolvedDrawCommonObjectsStorage& operator=(const AllInvolvedDrawCommonObjectsStorage&) = delete;
};
using AllInvolvedDrawCommonObjectsInitializer = std::function<void(IDrawBuffer*, IDrawCommon*)>;
using TaggedAllInvolvedDrawCommonObjectsInitializer =
    std::pair<DrawCommonHelper::RenderType, AllInvolvedDrawCommonObjectsInitializer>;

/**
 * @brief 从栈内存中使用对应RenderType的函数初始化渲染器和缓冲器
 *
 * @param ref_object_storage 栈内存对象，其中包含具有该内存独占所有权的指针
 * @param render_type 渲染器类型枚举
 * @param initializer_list 渲染器类型枚举和对应的初始化函数的总的集合
 * @return std::tuple<IDrawBuffer*, IDrawCommon*> 不具有所有权的裸指针
 */
auto GetInterfaceFromAllInvolvedDrawCommonObjects(
    AllInvolvedDrawCommonObjectsStorage& ref_object_storage,
    DrawCommonHelper::RenderType render_type,
    std::initializer_list<TaggedAllInvolvedDrawCommonObjectsInitializer> initializer_list)
    -> std::tuple<IDrawBuffer*, IDrawCommon*>;
