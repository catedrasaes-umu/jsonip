#ifndef JSONIP_HOLDER_HPP
#define JSONIP_HOLDER_HPP

namespace jsonip
{
    struct holder
    {
        holder() : t() {}
        holder(const holder& o) : t(o.t) {}

        template <typename T>
        holder(T* t_)
            : t(t_)
        {
        }

        bool valid() const { return !!t; }

        template <typename T>
        T& get()
        {
            return *reinterpret_cast<T*>(t);
        }

    private:
        void* t;
    };

} // namespace jsonip

#endif // JSONIP_HOLDER_HPP
