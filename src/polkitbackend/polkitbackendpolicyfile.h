/*
 * Copyright (C) 2017 Ikey Doherty
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Author: Ikey Doherty <ikey@solus-project.com>
 */

#if !defined(_POLKIT_BACKEND_COMPILATION)                                     \
    && !defined(_POLKIT_BACKEND_INSIDE_POLKIT_BACKEND_H)
#error                                                                        \
    "Only <polkitbackend/polkitbackend.h> can be included directly, this file may disappear or change contents."
#endif

#ifndef __POLKIT_BACKEND_POLICY_FILE_H
#define __POLKIT_BACKEND_POLICY_FILE_H

#include <glib.h>

/**
 * PolicyFileContraints are set per policy to ensure we'll only match
 * for explicitly set fields, as opposed to testing the default values
 */
typedef enum {
  PF_CONSTRAINT_MIN = 1 << 0,
  PF_CONSTRAINT_ACTIONS = 1 << 1,
  PF_CONSTRAINT_ACTION_CONTAINS = 1 << 2,
  PF_CONSTRAINT_SUBJECT_ACTIVE = 1 << 3,
  PF_CONSTRAINT_SUBJECT_LOCAL = 1 << 4,
  PF_CONSTRAINT_UNIX_GROUPS = 1 << 5,
  PF_CONSTRAINT_UNIX_NAMES = 1 << 6,
  PF_CONSTRAINT_NET_GROUPS = 1 << 7,
  PF_CONSTRAINT_RESULT = 1 << 8,
  PF_CONSTRAINT_RESULT_INVERSE = 1 << 9,
} PolicyFileConstraints;

/**
 * Valid responses we expect to see returned by each rule. This ensures we
 * have ahead-of-time rule validation.
 */
typedef enum {
  PK_RESPONSE_NO = 0,
  PK_RESPONSE_YES,
  PK_RESPONSE_AUTH_SELF,
  PK_RESPONSE_AUTH_SELF_KEEP,
  PK_RESPONSE_AUTH_ADMIN,
  PK_RESPONSE_AUTH_ADMIN_KEEP,
  PK_RESPONSE_UNHANDLED,
} PolkitResponse;

/**
 * Each file may have multiple policies defined, which are chained
 */
typedef struct Policy
{
  gchar *id;           /**<ID for this particular policy */
  struct Policy *next; /**<Next policy in this chain */

  gchar **actions; /**<Matched action IDs for Actions */
  gsize n_actions;

  gchar **action_contains; /**<Substring action IDs for Actions */
  gsize n_action_contains;

  gchar **unix_groups; /**<Unix groups for InUnixGroups */
  gsize n_unix_groups;

  gchar **unix_names; /**<Unix usernames for InUserNames */
  gsize n_unix_names;

  gchar **net_groups; /**<Net groups for InNetGroups */
  gsize n_net_groups;

  gboolean require_active;
  gboolean require_local;

  PolkitResponse response;
  PolkitResponse response_inverse;

  unsigned int constraints; /**<Match constraints per the keyfile */
} Policy;

/**
 * PolicyFile is the "compiled" variant of a policykit plain-text rules
 * file, and is a light weight replacement for the traditional JavaScript
 * based rule files.
 *
 * All of the rule files must be well defined ahead of time to allow very
 * strict runtime comparisons, vs runtime *execution*.
 */
typedef struct PolicyFile
{
  struct PolicyFile *next; /**<Next PolicyFile in the chain */

  struct
  {
    Policy *normal; /**<Ordinary rules */
    Policy *admin;  /**<Specialist admin rules */
  } rules;
} PolicyFile;

/**
 * Attempt to load a PolicyFile from the given path
 * @err: If not NULL, any parsing error will be stored here
 */
PolicyFile *policy_file_new_from_path (const char *path, GError **err);

/**
 * Free any resources associated with a PolicyFile
 */
void policy_file_free (PolicyFile *file);

#endif /* __POLKIT_BACKEND_POLICY_FILE_H */
