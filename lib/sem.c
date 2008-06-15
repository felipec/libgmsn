/*
 * Copyright (C) 2006-2008 Felipe Contreras.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation version 2.1 of
 * the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "sem.h"

GSem*
g_sem_new ()
{
    GSem *sem;
    sem = g_new (GSem, 1);
    sem->mutex = g_mutex_new ();
    sem->cond = g_cond_new ();
    sem->count = 0;

    return sem;
}

void
g_sem_free (GSem *sem)
{
    g_mutex_free (sem->mutex);
    g_cond_free (sem->cond);
}

void
g_sem_up (GSem *sem)
{
    g_mutex_lock (sem->mutex);
    sem->count++;
    g_mutex_unlock (sem->mutex);
    g_cond_signal (sem->cond);
}

void
g_sem_down (GSem* sem)
{
    g_mutex_lock (sem->mutex);
    while (sem->count == 0)
        g_cond_wait (sem->cond, sem->mutex);
    sem->count--;
    g_mutex_unlock (sem->mutex);
}

